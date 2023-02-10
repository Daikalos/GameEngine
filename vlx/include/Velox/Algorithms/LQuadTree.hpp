#pragma once

#include <vector>
#include <memory>
#include <optional>

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
	class VELOX_API LQuadTree
	{
	private:
		struct QuadNode
		{
			int first_child	{-1};	// first sub-branch or first element index
			int count		{0};	// -1 for branch or it's a leaf and count means number of elements
			RectFloat rect;			// loose AABB
		};

		struct QuadElt
		{
			int id {0};				// the item id
			RectFloat rect;			// rectangle encompassing the item
		};

		struct QuadEltPtr
		{
			int element {-1};		// points to item in elements
			int next	{-1};		// points to next elt ptr, or -1 means end of items
		};

		static constexpr int CHILD_COUNT = 4;

	public:
		LQuadTree(const RectFloat& root_rect, int max_elements = 16, int max_depth = 8);

	public:
		int Insert(const int id, const RectFloat& rect);
		bool Erase(const int id, const RectFloat& rect);

		std::vector<int> Query(const RectFloat& rect);
		std::vector<int> Query(const sf::Vector2f& point);

		void Cleanup();

	private:
		void UpdateLeafAABB(const int leaf_index);
		void SplitLeaf(QuadNode& node);

		void NodeInsert(const int node_index, const sf::Vector2f& node_center, const int ptr_index, const sf::Vector2f& elt_center, const int depth);

		int FindLeaf(const sf::Vector2f& point);

	private:
		RectFloat m_root_rect;

		int m_free_node		{-1};
		int m_max_elements	{16}; // max elements before subdivision
		int	m_max_depth		{8};  // max depth before no more leaves will be created

		FreeVector<QuadElt>		m_elements;		// all the elements
		FreeVector<QuadEltPtr>	m_elements_ptr;	// all the element ptrs
		FreeVector<QuadNode>	m_nodes;
	};
}