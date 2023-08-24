#pragma once

#include <vector>
#include <shared_mutex>
#include <concepts>

#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

#include <Velox/Structures/FreeVector.hpp>

namespace vlx
{
	///	Quadtree based upon: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
	///
	template<std::equality_comparable T = int>
	class QuadTree
	{
	public:
		using ElementType	= T;
		using ValueType		= std::remove_const_t<T>;
		using SizeType		= int;

		static constexpr int CHILD_COUNT = 4;

	public:
		struct Element
		{		
			RectFloat rect;			// rectangle encompassing the item
			ValueType item;
		};

	private:
		struct Node
		{
			SizeType first_child {-1};	// points to first sub-branch or first element index
			SizeType count		 {0};	// -1 for branch or it's a leaf and count means number of elements
		};

		struct NodeReg
		{
			RectFloat rect;
			SizeType index {0};
			SizeType depth {0};
		};

		struct ElementPtr
		{
			SizeType element {-1};	// points to item in elements, not sure if even needed, seems to always be aligned anyways
			SizeType next	 {-1};	// points to next elt ptr, or -1 means end of items
		};

	public:
		QuadTree(const RectFloat& root_rect, int max_elements = 8, int max_depth = 8);

	public:
		/// Inserts given element into the quadtree.
		/// 
		/// \param Element: Element to insert containing the item and bounding rectangle.
		/// 
		/// \returns Index to element, can be used to directly access it when, e.g., erasing it from the tree.
		/// 
		template<typename... Args> requires std::constructible_from<T, Args...>
		int Insert(const RectFloat& rect, Args&&... args);

		/// Attempts to erase element from tree.
		/// 
		/// \param EltIdx: Index to element to erase.
		/// 
		/// \returns True if successfully removed the element, otherwise false.
		/// 
		bool Erase(SizeType elt_idx);

		/// Updates the given element with new data.
		/// 
		/// \param Index: index to element.
		/// \param Args: Data to update the current element.
		/// 
		/// \returns True if successfully updated the element, otherwise false.
		/// 
		template<typename... Args> requires std::constructible_from<T, Args...>
		bool Update(SizeType idx, Args&&... args);

		/// Retrieves an element.
		/// 
		/// \param Index: index to element.
		/// 
		NODISC auto Get(SizeType idx) -> ValueType&;

		/// Retrieves an element.
		/// 
		/// \param Index: index to element.
		/// 
		NODISC auto Get(SizeType idx) const -> const ValueType&;

		/// Retrieves the rectangle encompassing item.
		/// 
		/// \param Index: index to item.
		/// 
		NODISC auto GetRect(SizeType idx) const -> const RectFloat&;

		/// Queries the tree for elements.
		/// 
		/// \param Rect: Bounding rectangle where all the elements are contained.
		/// 
		/// \returns List of entities contained within the bounding rectangle.
		/// 
		NODISC auto Query(const RectFloat& rect) const -> std::vector<SizeType>;

		/// Queries the tree for elements.
		/// 
		/// \param Point: Point to search for overlapping elements.
		/// 
		/// \returns List of entities contained at the point.
		/// 
		NODISC auto Query(const Vector2f& point) const-> std::vector<SizeType>;

		/// Performs a lazy cleanup of the tree; can only be called if erase has been used.
		/// 
		void Cleanup();

		/// Clears the tree
		/// 
		void Clear();

	private:
		void NodeInsert(const NodeReg& node, SizeType elt_idx);
		void LeafInsert(const NodeReg& node, SizeType elt_idx);

		auto FindLeaves(const NodeReg& node, const RectFloat& rect) const -> std::vector<NodeReg>;

		static bool IsLeaf(const Node& node);
		static bool IsBranch(const Node& node);

	private:
		FreeVector<Element>		m_elements;		// all the elements
		FreeVector<ElementPtr>	m_elements_ptr;	// all the element ptrs
		FreeVector<Node>		m_nodes;

		RectFloat	m_root_rect;

		SizeType	m_free_node			{-1};
		SizeType	m_max_elements		{8}; // max elements before subdivision
		SizeType	m_max_depth			{8};  // max depth before no more leaves will be created

		mutable std::vector<bool> m_visited;
		mutable std::shared_mutex m_mutex;
	};

	template<std::equality_comparable T>
	inline QuadTree<T>::QuadTree(const RectFloat& root_rect, int max_elements, int max_depth)
		: m_root_rect(root_rect), m_max_elements(max_elements), m_max_depth(max_depth)
	{
		m_nodes.emplace();
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline int QuadTree<T>::Insert(const RectFloat& rect, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		if (!m_root_rect.Overlaps(rect)) // dont attempt to add if outside boundary
			return -1;

		const auto idx = static_cast<SizeType>(m_elements.emplace(rect, std::forward<Args>(args)...));
		NodeInsert({ m_root_rect, 0, 0 }, idx);

		return idx;
	}

	template<std::equality_comparable T>
	inline bool QuadTree<T>::Erase(SizeType elt_idx)
	{
		std::unique_lock lock(m_mutex);

		const RectFloat& rect = m_elements[elt_idx].rect;
		const auto& leaves = FindLeaves({ m_root_rect, 0, 0 }, rect);

		if (leaves.empty())
			return false;

		for (const auto& leaf : leaves)
		{
			Node& node = m_nodes[leaf.index];

			auto ptr_idx = node.first_child;
			auto prv_idx = -1;

			while (ptr_idx != -1 && m_elements_ptr[ptr_idx].element != elt_idx)
			{
				prv_idx = ptr_idx;
				ptr_idx = m_elements_ptr[ptr_idx].next;
			}

			if (ptr_idx != -1)
			{
				const auto next_index = m_elements_ptr[ptr_idx].next;

				if (prv_idx == -1)
					node.first_child = next_index;
				else
					m_elements_ptr[prv_idx].next = next_index;

				m_elements_ptr.erase(ptr_idx);

				--node.count;

				assert(node.count >= 0);
			}
		}

		m_elements.erase(elt_idx);

		return true;
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline bool QuadTree<T>::Update(SizeType idx, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		if (idx >= m_elements.size() || !m_elements.valid(idx))
			return false;

		m_elements[idx].item = T{std::forward<Args>(args)...};

		return true;
	}

	template<std::equality_comparable T>
	auto QuadTree<T>::Get(SizeType idx) -> ValueType&
	{
		return m_elements[idx].item;
	}

	template<std::equality_comparable T>
	auto QuadTree<T>::Get(SizeType idx) const -> const ValueType&
	{
		return m_elements[idx].item;
	}

	template<std::equality_comparable T>
	inline auto QuadTree<T>::GetRect(SizeType idx) const -> const RectFloat&
	{
		return m_elements[idx].rect;
	}

	template<std::equality_comparable T>
	inline auto QuadTree<T>::Query(const RectFloat& rect) const -> std::vector<SizeType>
	{
		std::shared_lock lock(m_mutex);

		std::vector<SizeType> result;

		m_visited.resize(m_elements.size());

		for (const NodeReg& leaf : FindLeaves({ m_root_rect, 0, 0 }, rect))
		{
			for (auto child = m_nodes[leaf.index].first_child; child != -1;)
			{
				const auto& elt_ptr = m_elements_ptr[child];
				const auto& elt		= m_elements[elt_ptr.element];

				if (!m_visited[elt_ptr.element] && elt.rect.Overlaps(rect))
				{
					result.emplace_back(elt_ptr.element);
					m_visited[elt_ptr.element] = true;
				}

				child = elt_ptr.next;
			}
		}

		for (const auto elt_idx : result)
			m_visited[elt_idx] = false;

		return result;
	}

	template<std::equality_comparable T>
	inline auto QuadTree<T>::Query(const Vector2f& point) const -> std::vector<SizeType>
	{
		return Query(RectFloat(point.x, point.y, 0.f, 0.f));
	}

	template<std::equality_comparable T>
	inline void QuadTree<T>::Cleanup()
	{
		std::unique_lock lock(m_mutex);

		assert(!m_nodes.empty());

		std::vector<int> to_process;
		if (m_nodes[0].count == -1)
			to_process.emplace_back(0); // push root

		while (!to_process.empty())
		{
			Node& node = m_nodes[to_process.back()];
			to_process.pop_back();

			int num_empty = 0;
			for (int i = 0; i < CHILD_COUNT; ++i)
			{
				const int child_index	= node.first_child + i;
				const Node& child		= m_nodes[child_index];

				if (child.count == -1)
					to_process.emplace_back(child_index);
				else if (child.count == 0)
					++num_empty;
			}

			if (num_empty == CHILD_COUNT)
			{
				m_nodes.erase(node.first_child + 3);
				m_nodes.erase(node.first_child + 2);
				m_nodes.erase(node.first_child + 1);
				m_nodes.erase(node.first_child + 0);

				node = {}; // reset
			}
		}
	}

	template<std::equality_comparable T>
	inline void QuadTree<T>::Clear()
	{
		m_elements.clear();
		m_elements_ptr.clear();
		m_nodes.clear();

		m_free_node = {-1};
	}

	template<std::equality_comparable T>
	inline void QuadTree<T>::NodeInsert(const NodeReg& nr, SizeType elt_idx)
	{
		const RectFloat& rect = m_elements[elt_idx].rect;
		for (const auto& leaf : FindLeaves(nr, rect))
			LeafInsert(leaf, elt_idx);
	}

	template<std::equality_comparable T>
	inline void QuadTree<T>::LeafInsert(const NodeReg& nr, SizeType elt_idx)
	{
		Node& node = m_nodes[nr.index];
		node.first_child = m_elements_ptr.emplace(elt_idx, node.first_child);

		if (node.count == m_max_elements && nr.depth < m_max_depth)
		{
			std::vector<SizeType> elements;
			elements.reserve(m_max_elements);

			while (node.first_child != -1)
			{
				const ElementPtr& elt_ptr = m_elements_ptr[node.first_child];

				node.first_child = elt_ptr.next;
				m_elements_ptr.erase(node.first_child);

				elements.emplace_back(elt_ptr.element);
			}

			const auto fc = m_nodes.emplace();
			m_nodes.emplace();
			m_nodes.emplace();
			m_nodes.emplace();

			node.first_child = fc;
			node.count = -1; // set as branch

			for (const auto elt : elements)
				NodeInsert(nr, elt);
		}
		else
			++node.count;
	}

	template<std::equality_comparable T>
	inline auto QuadTree<T>::FindLeaves(const NodeReg& nr, const RectFloat& rect) const -> std::vector<NodeReg>
	{
		std::vector<NodeReg> leaves;
		std::vector<NodeReg> to_process;

		to_process.emplace_back(nr);

		while (!to_process.empty())
		{
			const NodeReg nd = to_process.back();
			to_process.pop_back();

			if (IsLeaf(m_nodes[nd.index]))
				leaves.emplace_back(nd);
			else
			{
				const auto fc	= m_nodes[nd.index].first_child;
				const auto hx	= nd.rect.width / 2.0f;
				const auto hy	= nd.rect.height / 2.0f;
				const auto l	= nd.rect.left - hx;
				const auto t	= nd.rect.top - hy;
				const auto r	= nd.rect.left + hx;
				const auto b	= nd.rect.top + hy;

				if (rect.top <= nd.rect.top)
				{
					if (rect.left <= nd.rect.left)
						to_process.emplace_back(RectFloat(l, t, hx, hy), fc + 0, nd.depth + 1);
					if (rect.Right() > nd.rect.left)
						to_process.emplace_back(RectFloat(r, t, hx, hy), fc + 1, nd.depth + 1);
				}
				if (rect.Bottom() > nd.rect.top)
				{
					if (rect.left <= nd.rect.left)
						to_process.emplace_back(RectFloat(l, b, hx, hy), fc + 2, nd.depth + 1);
					if (rect.Right() > nd.rect.left)
						to_process.emplace_back(RectFloat(r, b, hx, hy), fc + 3, nd.depth + 1);
				}
			}
		}

		return leaves;
	}

	template<std::equality_comparable T>
	inline bool QuadTree<T>::IsLeaf(const Node& node)
	{
		return node.count != -1;
	}

	template<std::equality_comparable T>
	inline bool QuadTree<T>::IsBranch(const Node& node)
	{
		return node.count == -1;
	}
}