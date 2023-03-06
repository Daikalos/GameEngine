#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <random>
#include <chrono>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

///	General utility class for arithmetic numbers
/// 
namespace vlx::au
{
	template<std::floating_point T = float>
	static constexpr T PI = T(M_PI);

	template<std::floating_point T>
	NODISC static constexpr T ToRadians(const T degrees) // unlikely to use integrals with radians or degrees
	{
		return degrees * (PI<T> / T(180.0));
	}

	template<std::floating_point T>
	NODISC static constexpr T ToDegrees(const T radians)
	{
		return radians * (T(180.0) / PI<T>);
	}

	template<Arithmetic T>
	NODISC static constexpr auto Lerp(const T a, const T b, const double f)
	{
		return (a * (1.0 - f)) + (b * f);
	}

	template<Arithmetic T>
	NODISC static constexpr T Pow(const T base, const std::int32_t exponent)
	{
		if (exponent < 0)
			return Pow(1 / base, -exponent);

		if (exponent == 0)
			return 1;

		if (exponent % 2 == 0)
			return Pow(base, exponent / 2) * Pow(base, exponent / 2);

		return base * Pow(base, (exponent - 1) / 2) * Pow(base, (exponent - 1) / 2);
	}

	template<Arithmetic T>
	NODISC static constexpr T Sq(const T base)
	{
		return Pow(base, 2);
	}

	template<Arithmetic T>
	NODISC static constexpr auto SqrtSq(const T v0, const T v1)
	{
		return std::sqrt(Sq(v0) + Sq(v1));
	}

	template<Arithmetic T>
	NODISC static constexpr int Sign(const T val)
	{
		return (val < T()) ? -1 : 1;
	}

	template<std::floating_point T>
	NODISC static constexpr auto Equal(const T a, const T b, const float epsilon = FLT_EPSILON)
	{
		return std::abs(a - b) <= epsilon;
	}

	template<std::integral T>
	NODISC static constexpr auto Wrap(const T val, const T min, const T max)
	{
		if (val > min && val < max)
			return val;

		const auto range_size = max - min;

		if (val < min)
			return max - ((min - val) % range_size);

		return min + ((val - min) % range_size);
	}

	template<std::floating_point T>
	NODISC static constexpr auto Wrap(T val, const T min, const T max)
	{
		if (val > min && val < max)
			return val;

		const auto range_size = max - min;

		if (val < min)
			return max - std::fmod(min - val, max - min);

		return min + std::fmod(val - min, max - min);
	}

	template<Arithmetic T>
	NODISC static constexpr auto MapToRange(const T val, const T minIn, const T maxIn, const T minOut, const T maxOut)
	{
		double x = (val - minIn) / (maxIn - minIn);
		return minOut + (maxOut - minOut) * x;
	}

	template<Arithmetic T>
	NODISC static constexpr auto SetPrecision(const T val, const int places)
	{
		double n = Pow(10.0, places);
		return std::round(val * n) / n;
	}
}
