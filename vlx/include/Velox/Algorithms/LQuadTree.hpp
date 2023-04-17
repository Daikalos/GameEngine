#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <shared_mutex>

#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

#include "FreeVector.hpp"
#include "SmallVector.hpp"

namespace vlx
{
	///	Loose quadtree based upon: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
	///
	template<std::equality_comparable T = int>
	class LQuadTree
	{
	public:
		using element_type = T;
		using value_type = std::remove_const_t<T>;

		static constexpr int CHILD_COUNT = 4;

	public:
		struct Element
		{		
			RectFloat rect;			// rectangle encompassing the item
			T item;
		};

	private:
		struct Node
		{
			RectFloat rect;			// loose rect
			int first_child	{-1};	// points to first sub-branch or first element index
			int count		{0};	// -1 for branch or it's a leaf and count means number of elements
		};

		struct ElementPtr
		{
			int elt_idx {-1};		// points to item in elements, not sure if even needed, seems to always be aligned anyways
			int next	{-1};		// points to next elt ptr, or -1 means end of items
		};

	public:
		LQuadTree(const RectFloat& root_rect, int max_elements = 8, int max_depth = 8);

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
		bool Erase(const int elt_idx);

		/// Attempts to erase element from tree.
		/// 
		/// \param Element: Element to erase.
		/// 
		/// \returns True if successfully removed the element, otherwise false.
		/// 
		bool Erase(const Element& element);

		/// Updates the given element with new data.
		/// 
		/// \param Index: index to element.
		/// \param Args: Data to update the current element.
		/// 
		/// \returns True if successfully updated the element, otherwise false.
		/// 
		template<typename... Args> requires std::constructible_from<T, Args...>
		bool Update(const int idx, Args&&... args);

		/// Retrieves an element.
		/// 
		/// \param Index: index to element.
		/// 
		NODISC T& Get(const int idx);

		/// Retrieves an element.
		/// 
		/// \param Index: index to element.
		/// 
		NODISC const T& Get(const int idx) const;

		/// Queries the tree for elements.
		/// 
		/// \param Rect: Bounding rectangle where all the elements are contained.
		/// 
		/// \returns List of entities contained within the bounding rectangle.
		/// 
		NODISC auto Query(const RectFloat& rect) const -> std::vector<Element>;

		/// Queries the tree for elements.
		/// 
		/// \param Point: Point to search for overlapping elements.
		/// 
		/// \returns List of entities contained at the point.
		/// 
		NODISC auto Query(const Vector2f& point) const-> std::vector<Element>;

		/// Performs a cleanup of the tree; can only be called if erase has been used.
		/// 
		void Cleanup();

		/// Completely clears the tree
		/// 
		void Clear();

	private:
		void EltInsert(const Vector2f& elt_center, const int ptr_idx);
		void SplitLeaf(Node& node);
		int FindLeaf(const Vector2f& point) const;
		void UpdateRect(Node& node, const RectFloat& elt_rect);

	private:
		FreeVector<Element>		m_elements;		// all the elements
		FreeVector<ElementPtr>	m_elements_ptr;	// all the element ptrs
		std::vector<Node>		m_nodes;

		RectFloat m_root_rect;

		int		m_free_node			{-1};
		int		m_max_elements		{8}; // max elements before subdivision
		int		m_max_depth			{8};  // max depth before no more leaves will be created
		bool	m_cleanup_required	{false}; 

		mutable std::shared_mutex m_mutex;
	};

	template<std::equality_comparable T>
	inline LQuadTree<T>::LQuadTree(const RectFloat& root_rect, int max_elements, int max_depth)
		: m_root_rect(root_rect), m_max_elements(max_elements), m_max_depth(max_depth)
	{
		m_nodes.emplace_back();
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline int LQuadTree<T>::Insert(const RectFloat& rect, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		if (!m_root_rect.Contains(rect.Center()))
			return -1;

		const auto idx = m_elements.emplace(rect, T(std::forward<Args>(args)...));
		EltInsert(rect.Center(), m_elements_ptr.emplace(idx));

		return idx;
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Erase(const int elt_idx)
	{
		std::unique_lock lock(m_mutex);

		assert(m_elements.valid(elt_idx));
		const auto& element = m_elements[elt_idx];

		int leaf_node_index = FindLeaf(element.rect.Center());
		Node& node = m_nodes[leaf_node_index];

		assert(node.count != -1); // we should have reached a leaf

		if (node.count == 0)
			return false;

		int* cur = &node.first_child, i = 0;
		while (*cur != -1) // search through all elements
		{
			i = *cur;

			if (elt_idx == m_elements_ptr[i].elt_idx)
			{
				*cur = m_elements_ptr[i].next;

				m_elements.erase(m_elements_ptr[i].elt_idx);
				m_elements_ptr.erase(i);

				--node.count;

				assert(node.count >= 0);

				m_cleanup_required = true;

				// Cleanup will possibly have to be called after this to work on corrected data

				return true;
			}

			cur = &m_elements_ptr[i].next;
		}

		return false;
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Erase(const Element& element)
	{
		std::unique_lock lock(m_mutex);

		int leaf_node_index = FindLeaf(element.rect.Center());
		Node& node = m_nodes[leaf_node_index];

		assert(node.count != -1); // we should have reached a leaf

		if (node.count == 0)
			return false;

		int* cur = &node.first_child, i = 0;
		while (*cur != -1) // search through all elements
		{
			i = *cur;

			Element& elt = m_elements[m_elements_ptr[i].elt_idx];
			if (elt.item == element.item)
			{
				*cur = m_elements_ptr[i].next;

				m_elements.erase(m_elements_ptr[i].elt_idx);
				m_elements_ptr.erase(i);

				--node.count;

				assert(node.count >= 0);

				m_cleanup_required = true;

				// Cleanup will possibly have to be called after this to work on corrected data

				return true;
			}

			cur = &m_elements_ptr[i].next;
		}

		return false;
	}

	template<std::equality_comparable T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline bool LQuadTree<T>::Update(const int idx, Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		if (idx >= m_elements.size() || !m_elements.valid(idx))
			return false;

		m_elements[idx].item = T(std::forward<Args>(args)...);

		return true;
	}

	template<std::equality_comparable T>
	T& LQuadTree<T>::Get(const int idx)
	{
		return m_elements[idx].item;
	}

	template<std::equality_comparable T>
	const T& LQuadTree<T>::Get(const int idx) const
	{
		return m_elements[idx].item;
	}

	template<std::equality_comparable T>
	inline auto LQuadTree<T>::Query(const RectFloat& rect) const -> std::vector<Element>
	{
		std::shared_lock lock(m_mutex);

		std::vector<Element> result{};

		SmallVector<int, 64> to_process;
		to_process.push_back(0); // push root

		while (!to_process.empty())
		{
			const Node& node = m_nodes[to_process.back()];
			to_process.pop_back();

			if (!rect.Overlaps(node.rect))
				continue;

			if (node.count != -1) // leaf
			{
				int child = node.first_child;
				while (child != -1) // iterate over all children
				{
					const auto& elt_ptr = m_elements_ptr[child];
					const auto& elt		= m_elements[elt_ptr.elt_idx];

					if (elt.rect.Overlaps(rect))
						result.push_back(elt);

					child = elt_ptr.next;
				}
			}
			else // it's a branch
			{
				for (int i = 0; i < CHILD_COUNT; ++i)
					to_process.push_back(node.first_child + i);
			}
		}

		return result;
	}

	template<std::equality_comparable T>
	inline auto LQuadTree<T>::Query(const Vector2f& point) const -> std::vector<Element>
	{
		std::shared_lock lock(m_mutex);

		std::vector<Element> result{};

		SmallVector<int> to_process;
		to_process.push_back(0); // push root

		while (!to_process.empty())
		{
			const Node& node = m_nodes[to_process.back()];
			to_process.pop_back();

			if (!node.rect.Contains(point))
				continue;

			if (node.count != -1) // leaf
			{
				int child = node.first_child;
				while (child != -1) // iterate over all children
				{
					const auto& elt_ptr = m_elements_ptr[child];
					const auto& elt		= m_elements[elt_ptr.elt_idx];

					if (elt.rect.Contains(point))
						result.emplace_back(elt);

					child = elt_ptr.next;
				}
			}
			else // branch
			{
				for (int i = 0; i < CHILD_COUNT; ++i)
					to_process.push_back(node.first_child + i);
			}
		}

		return result;
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::Cleanup()
	{
		std::unique_lock lock(m_mutex);

		if (!m_cleanup_required)
			return;

		SmallVector<int> to_process;
		to_process.push_back(0); // push root

		std::vector<RectFloat*> rects;

		while (!to_process.empty())
		{
			Node& node = m_nodes[to_process.back()];

			if (node.count == -1) // branch with elements that need to be processed
			{
				node.count = -2;
				for (int i = 0; i < CHILD_COUNT; ++i)
					to_process.push_back(node.first_child + i);
			}
			else if (node.count == -2) // branch with elements that has been processed
			{
				to_process.pop_back();

				bool init = false;
				for (int i = 0; i < CHILD_COUNT; ++i) // check all four children
				{
					const RectFloat* rect = rects.back();

					if (rect != nullptr)
					{
						if (!init)
						{
							node.rect = *rect;
							init = true;
						}
						else node.rect = node.rect.Union(*rect);
					}

					rects.pop_back();
				}

				if (init) // this node is not empty
				{
					node.count = -1; // set to branch
					rects.push_back(&node.rect); // push this node rect
				}
				else // empty, reset
				{
					m_nodes[node.first_child].first_child = m_free_node; // set new free node
					m_free_node = node.first_child;

					node = {}; // reset to default values

					rects.push_back(nullptr); // this node rect is considered null
				}
			}
			else // leaf
			{
				to_process.pop_back();

				if (node.count == 0) // this leaf is empty
				{
					assert(node.first_child == -1);

					rects.push_back(nullptr); // push empty rect
					node.rect = {}; // reset rect
				}
				else // this leaf has elements, update rect
				{
					int child = node.first_child;
					assert(child != -1);

					node.rect = m_elements[m_elements_ptr[child].elt_idx].rect;
					child = m_elements_ptr[child].next;

					while (child != -1)
					{
						node.rect = node.rect.Union(m_elements[m_elements_ptr[child].elt_idx].rect);
						child = m_elements_ptr[child].next;
					}

					rects.push_back(&node.rect);
				}
			}
		}

		m_cleanup_required = false;
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::Clear()
	{
		m_free_node = {-1};

		m_elements.clear();
		m_elements_ptr.clear();
		m_nodes.clear();
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::SplitLeaf(Node& node)
	{
		assert(node.count != -1); // cant split branch

		node.count = -1; // now a branch

		if (m_free_node != -1)
		{
			node.first_child = m_free_node;
			m_free_node = m_nodes[node.first_child].first_child;

			for (int i = 0; i < CHILD_COUNT; ++i)
				m_nodes[node.first_child + i] = {};
		}
		else
		{
			node.first_child = static_cast<int>(m_nodes.size());
			for (int i = 0; i < CHILD_COUNT; ++i)
				m_nodes.emplace_back();
		}
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::EltInsert(const Vector2f& elt_center, const int ptr_idx)
	{
		struct ElementReg // elements to process
		{
			Vector2f point;
			int idx		{0};
		};

		struct NodeReg // nodes to process
		{
			RectFloat rect;
			int idx		{0};
			int depth	{0};
		};

		std::vector<ElementReg> elements;
		std::vector<NodeReg> nodes;

		elements.emplace_back(elt_center, ptr_idx);
		nodes.emplace_back(m_root_rect, 0, 1);

		while (!elements.empty() && !nodes.empty())
		{
			const ElementReg& elt_reg = elements.back();
			const NodeReg& node_reg = nodes.back();

			Node& node = m_nodes[node_reg.idx];
			if (node_reg.rect.Contains(elt_reg.point))
			{
				if (node.count == -1) // traverse branch
				{
					Vector2f center = node_reg.rect.Center();
					Vector2f half_extends = { node_reg.rect.width / 2.0f, node_reg.rect.height / 2.0f };

					if (elt_reg.point.x >= center.x)
					{
						if (elt_reg.point.y >= center.y)
						{
							nodes.emplace_back(RectFloat(center.x, center.y,
								half_extends.x, half_extends.y), node.first_child + 3, node_reg.depth + 1);
						}
						else
						{
							nodes.emplace_back(RectFloat(center.x, center.y - half_extends.y,
								half_extends.x, half_extends.y), node.first_child + 2, node_reg.depth + 1);
						}
					}
					else
					{
						if (elt_reg.point.y >= center.y)
						{
							nodes.emplace_back(RectFloat(center.x - half_extends.x, center.y,
								half_extends.x, half_extends.y), node.first_child + 1, node_reg.depth + 1);
						}
						else
						{
							nodes.emplace_back(RectFloat(center.x - half_extends.x, center.y - half_extends.y,
								half_extends.x, half_extends.y), node.first_child + 0, node_reg.depth + 1);
						}
					}
				}
				else if (node.count < m_max_elements || node_reg.depth == m_max_depth) // insert element into node
				{
					++node.count;

					m_elements_ptr[elt_reg.idx].next = node.first_child;
					node.first_child = elt_reg.idx;

					UpdateRect(m_nodes[node_reg.idx], m_elements[m_elements_ptr[elt_reg.idx].elt_idx].rect);

					elements.pop_back();
				}
				else // otherwise, make space by splitting leaf and reinserting current elements
				{
					assert(node_reg.depth < m_max_depth); // we should only ever split leaf if above depth

					int child = node.first_child;
					while (child != -1)
					{
						elements.emplace_back(m_elements[m_elements_ptr[child].elt_idx].rect.Center(), child);
						child = m_elements_ptr[child].next;
					}

					SplitLeaf(node);
				}
			}
			else
			{
				nodes.pop_back();
			}
		}
	}

	template<std::equality_comparable T>
	inline int LQuadTree<T>::FindLeaf(const Vector2f& point) const
	{
		Vector2f offset	{ m_root_rect.width / 2.0f, m_root_rect.height / 2.0f };
		Vector2f node_center = m_root_rect.Center();

		int node_index = 0;
		while (m_nodes[node_index].count == -1) // continue on branches, will exit when leaf
		{
			const Node& node = m_nodes[node_index];
			offset /= 2.0f;

			if (point.x >= node_center.x)
			{
				if (point.y >= node_center.y) // bottom right
				{
					node_index = node.first_child + 3;
					node_center += offset;
				}
				else // top right
				{
					node_index = node.first_child + 2;
					node_center += { offset.x, -offset.y };
				}
			}
			else
			{
				if (point.y >= node_center.y) // bottom left
				{
					node_index = node.first_child + 1;
					node_center += { -offset.x, offset.y };
				}
				else // top left
				{
					node_index = node.first_child + 0;
					node_center -= offset;
				}
			}
		}

		return node_index;
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::UpdateRect(Node& node, const RectFloat& elt_rect)
	{
		node.rect = (node.count == 1) ? elt_rect : node.rect.Union(elt_rect);
	}
}