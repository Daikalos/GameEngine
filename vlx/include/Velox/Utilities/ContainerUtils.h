#pragma once

#include <vector>

#include "Concepts.h"

namespace vlx
{
	namespace cu
	{
		template<class T, std::equality_comparable_with<T> U>
		[[nodiscard]] static constexpr auto Erase(std::vector<T>& vector, const U& compare)
		{
			auto it = std::find_if(vector.begin(), vector.end(), compare);

			if (it == vector.end())
				return it;

			vector.erase(it);

			return it;
		}

		template<class T, class Func>
		[[nodiscard]] static constexpr auto Erase(std::vector<T>& vector, Func&& pred)
		{
			auto it = std::find_if(vector.begin(), vector.end(), pred);

			if (it == vector.end()) // do not erase if not found
				return it;

			vector.erase(it);

			return it;
		}

		template<class T, std::equality_comparable_with<T> U>
		[[nodiscard]] static constexpr auto SwapPop(std::vector<T>& vector, const U& compare)
		{
			auto it = std::find_if(vector.begin(), vector.end(), compare);

			if (it == vector.end())
				return it;
				
			std::iter_swap(it, vector.back());
			vector.pop_back();

			return it;
		}

		template<class T, class Func>
		[[nodiscard]] static constexpr auto SwapPop(std::vector<T>& vector, Func&& pred)
		{
			auto it = std::find_if(vector.begin(), vector.end(), pred);

			if (it == vector.end())
				return it;

			std::iter_swap(it, vector.back());
			vector.pop_back();

			return it;
		}
	}
}