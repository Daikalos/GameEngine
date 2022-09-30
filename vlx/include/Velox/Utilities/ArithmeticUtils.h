#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <random>
#include <chrono>

#include "Concepts.h"

////////////////////////////////////////////////////////////
// General utility class for arithmetic numbers
////////////////////////////////////////////////////////////
namespace vlx::au
{
	template<Arithmetic T>
	constexpr T PI = T(M_PI);

	template<std::floating_point T>
	[[nodiscard]] static constexpr T ToRadians(const T degrees) // unlikely to use integrals with radians or degrees
	{
		return degrees * (PI<T> / T(180.0));
	}

	template<std::floating_point T>
	[[nodiscard]] static constexpr T ToDegrees(const T radians)
	{
		return radians * (T(180.0) / PI<T>);
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto lerp(const T a, const T b, const double f)
	{
		return (a * (1.0 - f)) + (b * f);
	}

	template<std::integral T>
	[[nodiscard]] static constexpr auto Wrap(T val, const T min, const T max)
	{
		if (val > min && val < max)
			return val;

		const auto range_size = max - min;

		if (val < min)
			return max - ((min - val) % range_size);

		return min + ((val - min) % range_size);
	}
	template<std::floating_point T>
	[[nodiscard]] static constexpr auto Wrap(T val, const T min, const T max)
	{
		if (val > min && val < max)
			return val;

		const auto range_size = max - min;

		if (val < min)
			return max - std::fmod(min - val, max - min);

		return min + std::fmod(val - min, max - min);
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto MapToRange(const T val, const T minIn, const T maxIn, const T minOut, const T maxOut)
	{
		double x = (val - minIn) / (maxIn - minIn);
		return minOut + (maxOut - minOut) * x;
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto SetPrecision(const T val, const int places)
	{
		double n = std::pow(10.0, (double)places);
		return std::round(val * n) / n;
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto Sign(const T val)
	{
		return (std::signbit(val) == 0) ? 1 : -1;
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto P2(const T val)
	{
		return val * val;
	}

	template<Arithmetic T>
	[[nodiscard]] static constexpr auto SP2(const T v0, const T v1)
	{
		return std::sqrt(P2(v0) + P2(v1));
	}
}
