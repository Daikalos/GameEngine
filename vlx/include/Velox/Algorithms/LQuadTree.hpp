#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <shared_mutex>

#include <SFML/System/Vector2.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utilities/Rectangle.hpp>
#include <Velox/Utilities/VectorUtils.h>

#include "FreeVector.hpp"
#include "SmallVector.hpp"

namespace vlx
{
	/// <summary>
	///		Loose quadtree based upon: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
	/// </summary>
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
			int element {-1};		// points to item in elements
			int next	{-1};		// points to next elt ptr, or -1 means end of items
		};

		static constexpr int CHILD_COUNT = 4;

	public:
		LQuadTree(const RectFloat& root_rect, int max_elements = 16, int max_depth = 8);

	public:
		NODISC int Insert(const Element& element);

		bool Erase(const int element);
		bool Erase(const Element& element);

		NODISC auto Query(const RectFloat& rect) const -> std::vector<Element>;
		NODISC auto Query(const sf::Vector2f& point) const-> std::vector<Element>;

		void Cleanup();
		void Clear();	// performs a complete cleanup

	private:
		void SplitLeaf(Node& node);
		void EltInsert(const int node_index, const sf::Vector2f& node_center, const int ptr_index, const sf::Vector2f& elt_center, const int depth);

		int FindLeaf(const sf::Vector2f& point) const;

	private:
		RectFloat m_root_rect;

		int m_free_node		{-1};
		int m_max_elements	{16}; // max elements before subdivision
		int	m_max_depth		{8};  // max depth before no more leaves will be created

		FreeVector<Element>		m_elements;		// all the elements
		FreeVector<ElementPtr>	m_elements_ptr;	// all the element ptrs
		FreeVector<Node>		m_nodes;

		mutable std::shared_mutex m_mutex;
	};

	template<std::equality_comparable T>
	inline LQuadTree<T>::LQuadTree(const RectFloat& root_rect, int max_elements, int max_depth)
		: m_root_rect(root_rect), m_max_elements(max_elements), m_max_depth(max_depth)
	{
		m_nodes.emplace();
	}

	template<std::equality_comparable T>
	inline int LQuadTree<T>::Insert(const Element& element)
	{
		std::unique_lock lock(m_mutex);

		ElementPtr elt_ptr{};

		const auto ptr = m_elements.emplace(element);
		elt_ptr.element = ptr;

		EltInsert(0, m_root_rect.Center(), m_elements_ptr.insert(elt_ptr), element.rect.Center(), 1);

		return ptr;
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Erase(const int element)
	{
		return Erase(m_elements.at(element));
	}

	template<std::equality_comparable T>
	inline bool LQuadTree<T>::Erase(const Element& element)
	{
		std::unique_lock lock(m_mutex);

		int leaf_node_index = FindLeaf(element.rect.Center());

		if (m_nodes[leaf_node_index].count == 0)
			return false;

		Node& node = m_nodes[leaf_node_index];

		int* cur = &node.first_child;
		while (*cur != -1) // search through all elements
		{
			ElementPtr& elt_ptr = m_elements_ptr[*cur];
			Element& elt = m_elements[elt_ptr.element];

			if (elt.item == element.item)
			{
				m_elements.erase(elt_ptr.element);

				const int temp = *cur;
				*cur = elt_ptr.next;

				m_elements_ptr.erase(temp);

				--node.count;

				// Cleanup will possibly have to be called after this to work on corrected data

				return true;
			}

			cur = &elt_ptr.next;
		}

		return false;
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
					const Element& elt = m_elements[elt_ptr.element];

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
	inline auto LQuadTree<T>::Query(const sf::Vector2f& point) const -> std::vector<Element>
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

		SmallVector<int, 128> to_process;
		to_process.push_back(0); // push root

		SmallVector<RectFloat*, 128> node_rects;

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
					const RectFloat* rect = node_rects.back();

					if (rect != nullptr)
					{
						if (!init)
						{
							node.rect = *rect;
							init = true;
						}
						else node.rect = node.rect.Union(*rect);
					}

					node_rects.pop_back();
				}

				if (init) // this node is not empty
				{
					node.count = -1;
					node_rects.push_back(&node.rect); // push this node rect
				}
				else // empty, reset
				{
					m_nodes[node.first_child].first_child = m_free_node; // set new free node
					m_free_node = node.first_child;

					node = {}; // reset to default values

					node_rects.push_back(nullptr); // this node rect is considered null
				}
			}
			else // leaf
			{
				to_process.pop_back();

				if (node.count == 0) // this leaf is empty
				{
					assert(node.first_child == -1);

					node_rects.push_back(nullptr); // push empty rect
					node.rect = {}; // reset rect
				}
				else // this leaf has elements, update rect
				{
					int child = node.first_child;
					assert(child != -1);

					node.rect = m_elements[m_elements_ptr[child].element].rect;
					child = m_elements_ptr[child].next;

					while (child != -1)
					{
						node.rect = node.rect.Union(m_elements[m_elements_ptr[child].element].rect);
						child = m_elements_ptr[child].next;
					}

					node_rects.push_back(&node.rect);
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
		if (m_free_node != -1)
		{
			node.first_child = m_free_node;
			m_free_node = m_nodes[node.first_child].first_child;

			for (int i = 0; i < CHILD_COUNT; ++i)
				m_nodes[node.first_child + i] = {};
		}
		else
		{
			node.first_child = m_nodes.size();
			for (int i = 0; i < CHILD_COUNT; ++i)
				m_nodes.emplace();
		}
	}

	template<std::equality_comparable T>
	inline void LQuadTree<T>::EltInsert(const int node_index, const sf::Vector2f& node_center, const int ptr_index, const sf::Vector2f& elt_center, const int depth)
	{
		sf::Vector2f offset
		{
			m_root_rect.width / (2 * (depth + 1)),
			m_root_rect.height / (2 * (depth + 1))
		};

		auto& node = m_nodes[node_index];

		if (node.count == -1) // branch
		{
			if (elt_center.x > node_center.x)
			{
				(elt_center.y > node_center.y) ?
					EltInsert(node.first_child + 3, { node_center.x + offset.x, node_center.y + offset.y }, ptr_index, elt_center, depth + 1) :
					EltInsert(node.first_child + 2, { node_center.x + offset.x, node_center.y - offset.y }, ptr_index, elt_center, depth + 1);
			}
			else
			{
				(elt_center.y > node_center.y) ?
					EltInsert(node.first_child + 1, { node_center.x - offset.x, node_center.y + offset.y }, ptr_index, elt_center, depth + 1) :
					EltInsert(node.first_child + 0, { node_center.x - offset.x, node_center.y - offset.y }, ptr_index, elt_center, depth + 1);
			}
		}
		else if (depth < m_max_depth && node.count == m_max_elements) // we have reached max elements, convert this to branch
		{
			node.count = -1;
			m_elements_ptr[ptr_index].next = node.first_child;

			SplitLeaf(node);

			int next{}, i = ptr_index;
			while (i != -1)
			{
				next = m_elements_ptr[i].next;
				EltInsert(node_index, node_center, i, m_elements[m_elements_ptr[i].element].rect.Center(), depth);
				i = next;
			}

			return;
		}
		else // normal insert if we have reached max depth or less than max elements
		{
			m_elements_ptr[ptr_index].next = node.first_child;
			node.first_child = ptr_index;
			++node.count;
		}

		const auto& elt_ptr = m_elements_ptr[ptr_index];
		const auto& elt = m_elements[elt_ptr.element];

		node.rect = (node.count == 1) ? elt.rect : node.rect.Union(elt.rect); // update aabb
	}

	template<std::equality_comparable T>
	inline int LQuadTree<T>::FindLeaf(const sf::Vector2f& point) const
	{
		sf::Vector2f offset{ m_root_rect.Width() / 2, m_root_rect.Height() / 2 };
		sf::Vector2f node_center = m_root_rect.Center();

		int node_index = 0;
		while (m_nodes[node_index].count == -1) // continue on branches, will exit when leaf
		{
			Node& node = m_nodes[node_index];
			offset /= 2.0f;

			if (point.x > node_center.x)
			{
				if (point.y > node_center.y)
				{
					node_index = node.first_child + 3;
					node_center += offset;
				}
				else
				{
					node_index = node.first_child + 2;
					node_center += { offset.x, -offset.y };
				}
			}
			else
			{
				if (point.y > node_center.y)
				{
					node_index = node.first_child + 1;
					node_center += { -offset.x, offset.y };
				}
				else
				{
					node_index = node.first_child + 0;
					node_center -= offset;
				}
			}
		}

		return node_index;
	}
}