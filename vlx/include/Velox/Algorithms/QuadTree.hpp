#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	static constexpr int CHILD_COUNT = 4;

	template<class T>
	class QuadTree
	{
	private:
		using Rect = sf::Rect<int>;

	private:
		struct QuadNode
		{
			int first_child	{-1};
			int count		{-1};
		};

		struct QuadElt
		{
			int id;
			Rect rect;
		};

		struct QuadEltNode
		{
			int next;
			int element;
		};

	public:
		QuadTree(int max_depth = 8);

	public:


	private:
		std::vector<QuadElt>		m_quads;
		std::vector<QuadEltNode>	m_elt_nodes;
		std::vector<QuadNode>		m_nodes;

		Rect m_root_rect;

		int m_free_node;
		int	m_max_depth{8};
	};
}