#pragma once

#include <type_traits>
#include <random>
#include <chrono>

namespace vlx
{
	namespace rnd
	{
		static thread_local std::mt19937_64 dre(std::chrono::steady_clock::now().time_since_epoch().count());

		template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		static constexpr T random(const T min, const T max)
		{
			std::uniform_real_distribution<T> uid(min, max);
			return uid(dre);
		}
		template<typename T, typename std::enable_if_t<!std::is_floating_point_v<T>, bool> = true>
		static constexpr T random(const T min, const T max)
		{
			std::uniform_int_distribution<T> uid(min, max);
			return uid(dre);
		}

		////////////////////////////////////////////////////////////
		// Creates a list of values up to size that are then 
		// shuffled and finally returned
		////////////////////////////////////////////////////////////
		template<typename T>
		static constexpr std::vector<T> random(const T size)
		{
			std::vector<T> result;
			result.reserve(size);

			for (T i = 0; i < size; ++i)
				result.emplace_back(i);

			std::shuffle(result.begin(), result.end(), dre);

			return result;
		}

		////////////////////////////////////////////////////////////
		// Returns a random T from the given set of arguments
		////////////////////////////////////////////////////////////
		template<typename T, typename... Args>
		static constexpr T random_arg(const T& arg0, Args&&... args)
		{
			std::vector<T> vector{ { arg0, std::forward<Args>(args)... } };
			return vector[random<size_t>(0LLU, vector.size() - 1)];
		}
	}
}