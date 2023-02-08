#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>

#include "FreeVector.hpp"

namespace vlx
{
	class VELOX_API QuadTree
	{
	private:
		struct QuadNode
		{
			int first_child	{-1};
			int count		{0};
		};

		struct QuadElt
		{
			int id;
			sf::IntRect rect;
		};

		struct QuadEltNode
		{
			int element {-1};
			int next	{-1};
		};

	public:
		QuadTree(const sf::IntRect& root_rect, int max_elements = 16, int max_depth = 8);

	public:
		void Insert(const QuadEltNode& elt);
		void Erase(const QuadEltNode& elt);

		std::optional<FreeVector<QuadEltNode>> Query(const sf::IntRect& rect);
		std::optional<FreeVector<QuadEltNode>> Query(const sf::Vector2i& point);

		void Cleanup();

	private:
		void NodeInsert(int index, int depth, const sf::Vector2i& centre, const sf::Vector2i& half_size, int element);
		void LeafInsert(int index, int depth, const sf::Vector2i& centre, const sf::Vector2i& half_size, int element);

		std::vector<int> FindLeaves(int index, int depth, const sf::Vector2i& centre, const sf::Vector2i& half_size, const sf::IntRect& rect);

	private:
		sf::IntRect m_root_rect;

		int m_free_node		{-1};
		int m_max_elements	{16}; // max elements before subdivision
		int	m_max_depth		{8};  // max depth before no more leaves will be created

		FreeVector<QuadElt>		m_quads;
		FreeVector<QuadEltNode>	m_elt_nodes;
		FreeVector<QuadNode>	m_nodes;
	};
}