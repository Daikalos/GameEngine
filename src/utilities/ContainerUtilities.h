#pragma once

#include <vector>
#include <FGE/Concepts.hpp>

namespace fge
{
	namespace cu
	{
		template<class T, std::equality_comparable_with<T> U>
		static bool Erase(std::vector<T>& vector, const U& val)
		{
			auto it = std::find_if(vector.begin(), vector.end(), [&val](const T& value)
				{ return val == value; });

			if (it == vector.end())
				return false;

			vector.erase(it);

			return true;
		}
	}
}