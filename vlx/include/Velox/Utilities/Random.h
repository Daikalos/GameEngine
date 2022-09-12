#pragma once

#include "Concepts.h"

#include <type_traits>
#include <random>
#include <chrono>
#include <array>
#include <numeric>

namespace vlx::rnd
{
	static thread_local std::mt19937_64 dre(std::chrono::steady_clock::now().time_since_epoch().count());

	template<std::floating_point T>
	[[nodiscard]] static constexpr T random(const T min, const T max)
	{
		std::uniform_real_distribution<T> uid(min, max);
		return uid(dre);
	}
	template<std::integral T>
	[[nodiscard]] static constexpr T random(const T min, const T max)
	{
		std::uniform_int_distribution<T> uid(min, max);
		return uid(dre);
	}

	////////////////////////////////////////////////////////////
	// Creates a list of values up to size that are then 
	// shuffled and finally returned
	////////////////////////////////////////////////////////////
	template<Arithmetic T>
	[[nodiscard]] static constexpr auto random(const T size)
	{
		std::array<T, size> result;

		std::iota(result.begin(), result.end(), 0);
		std::shuffle(result.begin(), result.end(), dre);

		return result;
	}

	////////////////////////////////////////////////////////////
	// Returns a random T from the given set of arguments
	////////////////////////////////////////////////////////////
	template<typename T, typename... Args>
	[[nodiscard]] static constexpr T random_arg(const T& arg0, Args&&... args) requires SameType<T, Args...>
	{
		const std::size_t size = sizeof...(args) + 1;
		const T arr[size] = { arg0, std::forward<Args>(args)... };
		return arr[random<std::size_t>(0LLU, size - 1)];
	}
}