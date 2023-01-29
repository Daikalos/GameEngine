#pragma once

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	template<class T>
	class QuadTree
	{
	private:
		using Quad = sf::Rect<int>;

	private:
		struct QuadNode
		{
			int first_child	{-1};
			int count		{-1};
		};

		struct QuadElt
		{
			int id;
			Quad quad;
		};

		struct QuadEltNode
		{
			int next;
			int element;
		};

	private:

	};
}