#pragma once

#include <vector>

#include <Velox/System/Vector2.hpp>

#include "FreeVector.hpp"

namespace vlx
{
	template<typename T>
	class KDTree
	{
	private:
		struct Node
		{
			int left	{-1};
			int right	{-1};
		};

		struct EltNode
		{
			int elt_ptr {-1};
		};

	public:
		FreeVector<T> m_elements;
		std::vector<Node> m_nodes;
	};
}