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
		using value_type = T;

	public:
		struct Element
		{
			T item;					
			RectFloat rect;			// rectangle encompassing the item
		};

	private:
		struct Node
		{
			int first_child	{-1};	// first sub-branch or first element index
			int count		{0};	// -1 for branch or it's a leaf and count means number of elements
			RectFloat rect;			// loose Box
		};

		struct ElementPtr
		{
			int elt_idx {-1};		// points to item in elements
			int next	{-1};		// points to next elt ptr, or -1 means end of items
		};

		static constexpr int CHILD_COUNT = 4;

	public:
		LQuadTree(const RectFloat& root_rect, int max_elements = 16, int max_depth = 8);

	public:
		auto Insert(const Element& element) -> const Element*;
		bool Erase(const Element& element);

		bool Update(const int element, const T& new_item);

		NODISC auto Query(const RectFloat& rect) const -> std::vector<Element>;
		NODISC auto Query(const Vector2f& point) const-> std::vector<Element>;

		void Cleanup();
		void Clear();	// performs a complete cleanup

	private:
		void SplitLeaf(Node& node);
		void EltInsert(const Vector2f& elt_center, const int ptr_idx);

		int FindLeaf(const Vector2f& point) const;

		void UpdateRect(Node& node, const RectFloat& elt_rect);

	private:
		RectFloat m_root_rect;

		int m_free_node		{-1};
		int m_max_elements	{16}; // max elements before subdivision
		int	m_max_depth		{8};  // max depth before no more leaves will be created
		int m_divisions		{0};

		FreeVector<Element>		m_elements;		// all the elements
		FreeVector<ElementPtr>	m_elements_ptr;	// all the element ptrs
		std::vector<Node>		m_nodes;

		mutable std::shared_mutex m_mutex;
	};

	template<std::equality_comparable T>
	inline LQuadTree<T>::LQuadTree(const RectFloat& root_rect, int max_elements, int max_depth)
		: m_root_rect(root_rect), m_max_elements(max_elements), m_max_depth(max_depth)
	{
		m_nodes.emplace_back();
	}

	template<std::equality_comparable T>
	inline auto LQuadTree<T>::Insert(const Element& element) -> const Element*
	{
		std::unique_lock lock(m_mutex);

		const auto ptr = m_elements.emplace(element);

		ElementPtr elt_ptr{};
		elt_ptr.elt_idx = ptr;

		EltInsert(element.rect.Center(), m_elements_ptr.insert(elt_ptr));

		return &m_elements[ptr];
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Erase(const Element& element)
	{
		std::unique_lock lock(m_mutex);

		int leaf_node_index = FindLeaf(element.rect.Center());
		Node& node = m_nodes[leaf_node_index];

		assert(node.count != -1);

		if (node.count == 0)
		{
			int test = FindLeaf(element.rect.Center());
			return false;
		}

		int* cur = &node.first_child;
		while (*cur != -1) // search through all elements
		{
			Element& elt = m_elements[m_elements_ptr[*cur].elt_idx];

			if (elt.item == element.item)
			{
				const int temp = *cur;
				*cur = m_elements_ptr[*cur].next;

				m_elements.erase(m_elements_ptr[temp].elt_idx);
				m_elements_ptr.erase(temp);

				--node.count;

				// Cleanup will possibly have to be called after this to work on corrected data

				return true;
			}

			cur = &m_elements_ptr[*cur].next;
		}

		int test = FindLeaf(element.rect.Center());

		return false;
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Update(const int element, const T& new_item)
	{
		std::unique_lock lock(m_mutex);

		if (!m_elements.valid(element))
			return false;

		m_elements[element].item = new_item;

		return true;
	}

	template<std::equality_comparable T>
	inline auto LQuadTree<T>::Query(const RectFloat& rect) const -> std::vector<Element>
	{
		std::shared_lock lock(m_mutex);

		std::vector<Element> result{};

		SmallVector<int, 128> to_process;
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
					const ElementPtr& elt_ptr = m_elements_ptr[child];
					const Element& elt = m_elements[elt_ptr.elt_idx];

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

		SmallVector<int, 128> to_process;
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
					const ElementPtr& elt_ptr = m_elements_ptr[child];
					const Element& elt = m_elements[elt_ptr.element];

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

		std::vector<int> to_process;
		to_process.push_back(0); // push root

		std::vector<RectFloat*> rects;

		while (!to_process.empty())
		{
			const int node_index = to_process.back();
			Node& node = m_nodes[node_index];

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
					node.count = -1;
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

			++m_divisions;
		}
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::EltInsert(const Vector2f& elt_center, const int ptr_idx)
	{
		struct NodeReg // nodes to process
		{
			RectFloat rect;
			int depth	{0};
			int idx		{0};
		};

		struct ElementReg // elements to process
		{
			Vector2f point;
			int idx		{0};
		};

		std::vector<ElementReg> to_process;
		std::vector<NodeReg> to_insert;

		to_process.emplace_back(elt_center, ptr_idx);
		to_insert.emplace_back(m_root_rect, 1, 0);

		while (!to_process.empty() && !to_insert.empty())
		{
			const ElementReg& elt_reg = to_process.back();
			const NodeReg& node_reg = to_insert.back();

			Node& node = m_nodes[node_reg.idx];
			if (node_reg.rect.Contains(elt_reg.point))
			{
				if (node.count == -1)
				{
					UpdateRect(m_nodes[node_reg.idx], m_elements[m_elements_ptr[elt_reg.idx].elt_idx].rect);

					Vector2f center = node_reg.rect.Center();
					Vector2f offset = { node_reg.rect.width / 2.0f, node_reg.rect.height / 2.0f };

					if (elt_reg.point.x > center.x)
					{
						if (elt_reg.point.y > center.y)
						{
							to_insert.emplace_back(RectFloat(center.x, center.y, offset.x, offset.y), 
								node.first_child + 3, node_reg.depth + 1);
						}
						else
						{
							to_insert.emplace_back(RectFloat(center.x, center.y - offset.y, offset.x, offset.y), 
								node.first_child + 2, node_reg.depth + 1);
						}
					}
					else
					{
						if (elt_reg.point.y > center.y)
						{
							to_insert.emplace_back(RectFloat(center.x - offset.x, center.y, offset.x, offset.y), 
								node.first_child + 1, node_reg.depth + 1);
						}
						else
						{
							to_insert.emplace_back(RectFloat(center.x - offset.x, center.y - offset.y, offset.x, offset.y), 
								node.first_child + 0, node_reg.depth + 1);
						}
					}
				}
				else if (node.count < m_max_elements || node_reg.depth == m_max_depth)
				{
					++node.count;

					UpdateRect(m_nodes[node_reg.idx], m_elements[m_elements_ptr[elt_reg.idx].elt_idx].rect);

					m_elements_ptr[elt_reg.idx].next = node.first_child;
					node.first_child = elt_reg.idx;

					to_process.pop_back();
				}
				else
				{
					int child = node.first_child;
					while (child != -1)
					{
						to_process.emplace_back(m_elements[m_elements_ptr[child].elt_idx].rect.Center(), child);
						child = m_elements_ptr[child].next;
					}

					SplitLeaf(node);
				}
			}
			else
			{
				to_insert.pop_back();
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

			if (point.x > node_center.x)
			{
				if (point.y > node_center.y) // bottom right
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
				if (point.y > node_center.y) // bottom left
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