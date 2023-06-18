#pragma once

#define _USE_MATH_DEFINES

#include <SFML/System/Angle.hpp>

#include <math.h>
#include <random>
#include <chrono>

#include <Velox/System/Concepts.h>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

///	General utility class for arithmetic numbers
/// 
namespace vlx::au
{
	template<std::floating_point T = float>
	inline constexpr T PI = T(M_PI);

	template<std::floating_point T = float>
	inline constexpr T PI_2 = T(M_PI);

	template<std::floating_point T>
	NODISC static constexpr T ToRadians(T degrees) // unlikely to use integrals with radians or degrees
	{
		return degrees * (PI<T> / T(180.0));
	}

	template<std::floating_point T>
	NODISC static constexpr T ToDegrees(T radians)
	{
		return radians * (T(180.0) / PI<T>);
	}

	template<Arithmetic T>
	NODISC static constexpr T Pow(T base, const int32 exponent)
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
	NODISC static constexpr T Sqr(T base)
	{
		return Pow(base, 2);
	}

	template<Arithmetic T>
	NODISC static constexpr auto SqrtSqr(T v0, T v1)
	{
		return std::sqrt(Sqr(v0) + Sqr(v1));
	}

	template<Arithmetic T>
	NODISC static constexpr int Sign(T val)
	{
		return (val < T()) ? -1 : 1;
	}

	template<std::floating_point T>
	NODISC static constexpr auto Equal(T a, T b, float epsilon = FLT_EPSILON)
	{
		return std::abs(a - b) <= epsilon;
	}

	template<std::integral T>
	NODISC static constexpr auto Wrap(T val, T max, T min = 0)
	{
		const auto range = max - min;

		if (val < min)
			return max - ((min - val) % range);

		if (val >= max)
			return min + ((val - min) % range);

		return val;
	}

	template<std::floating_point T>
	NODISC static constexpr auto Wrap(T val, T max, T min = 0)
	{
		const auto range = max - min;

		if (val < min)
			return max - std::fmod(min - val, range);

		if (val >= max)
			return min + std::fmod(val - min, range);

		return val;
	}

	template<std::integral T>
	NODISC static constexpr auto WrapLower(T val, T max, T min = 0)
	{
		if (val < min)
			return max - ((min - val) % (max - min));

		return val;
	}

	template<std::integral T>
	NODISC static constexpr auto WrapUpper(T val, T max, T min = 0)
	{
		if (val >= max)
			return min + ((val - min) % (max - min));

		return val;
	}

	template<std::floating_point T>
	NODISC static constexpr auto WrapLower(T val, T max, T min = 0)
	{
		if (val < min)
			return max - std::fmod(min - val, max - min);

		return val;
	}

	template<std::floating_point T>
	NODISC static constexpr auto WrapUpper(T val, T max, T min = 0)
	{
		if (val > max)
			return min + std::fmod(val - min, max - min);

		return val;
	}

	template<Arithmetic T>
	NODISC static constexpr auto MapToRange(T val, T minIn, T maxIn, T minOut, T maxOut)
	{
		double x = (val - minIn) / (maxIn - minIn);
		return minOut + (maxOut - minOut) * x;
	}

	template<Arithmetic T>
	NODISC static constexpr auto SetPrecision(T val, const int places)
	{
		double n = Pow(10.0, places);
		return std::round(val * n) / n;
	}

	template<Arithmetic T>
	NODISC static constexpr T Abs(T x) noexcept
	{
		return std::abs(x);
	}

	NODISC static float ShortestAngleDegrees(sf::Angle a, sf::Angle b)
	{
		return 180.0f - std::abs(std::fmodf(std::abs(b.asDegrees() - a.asDegrees()), 360.0f) - 180.0f);
	}

	NODISC static float ShortestAngleRadians(sf::Angle a, sf::Angle b)
	{
		return PI<> - std::abs(std::fmodf(std::abs(b.asRadians() - a.asRadians()), PI<> * 2) - PI<>);
	}

	template<std::floating_point T>
	NODISC static constexpr auto Lerp(T a, T b, float f)
	{
		return (a * (1.0f - f)) + (b * f);
	}

	NODISC static sf::Angle Lerp(sf::Angle a, sf::Angle b, float f)
	{
		return a + sf::degrees(ShortestAngleDegrees(a, b) * f);
	}
}
