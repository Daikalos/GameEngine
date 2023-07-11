#pragma once

#include <type_traits>
#include <random>
#include <vector>
#include <array>
#include <numeric>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include <Velox/Types.hpp>

namespace vlx::rnd
{
	inline thread_local std::mt19937_64 dre(std::random_device{}());

	NODISC inline float random()
	{
		std::uniform_real_distribution<float> uid(0.0f, 1.0f);
		return uid(dre);
	}

	template<FloatingPoint T>
	NODISC inline T random(T min, T max)
	{
		std::uniform_real_distribution<T> uid(min, max);
		return uid(dre);
	}
	template<Integral T>
	NODISC inline T random(T min, T max)
	{
		std::uniform_int_distribution<T> uid(min, max);
		return uid(dre);
	}

	///	Creates a numerically increasing list of values up to size that are then shuffled and finally returned
	/// 
	template<Arithmetic T>
	NODISC inline auto vector(T size)
	{
		std::vector<T> result;
		result.resize(size);

		std::iota(result.begin(), result.end(), 0);
		std::shuffle(result.begin(), result.end(), dre);

		return result;
	}

	///	Creates a numerically increasing list of values up to size that are then shuffled and finally returned
	/// 
	template<Arithmetic T, std::size_t N>
	NODISC inline auto array()
	{
		std::array<T, N> result;

		std::iota(result.begin(), result.end(), 0);
		std::shuffle(result.begin(), result.end(), dre);

		return result;
	}

	///	Returns a random T from the given set of arguments
	/// 
	template<typename T, typename... Args> requires IsSameType<T, Args...>
	NODISC inline T args(T& arg0, Args&&... args)
	{
		const std::size_t size = sizeof...(args) + 1;
		const T arr[size] = { arg0, std::forward<Args>(args)... };
		return arr[random<std::size_t>(0LLU, size - 1)];
	}

	inline void seed(uint64 seed = std::mt19937_64::default_seed)
	{
		dre.seed(seed);
	}
}