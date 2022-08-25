#pragma once

#define _USE_MATH_DEFINES

#include <Velox/Concepts.hpp>

#include <math.h>
#include <random>
#include <chrono>

namespace vlx
{
	////////////////////////////////////////////////////////////
	// General utility class
	////////////////////////////////////////////////////////////
	namespace util
	{
		template<std::floating_point T>
		static constexpr T ToRadians(const T degrees) // unlikely to use integrals with radians or degrees
		{
			return degrees * (M_PI / T(180.0));
		}

		template<std::floating_point T>
		static constexpr T ToDegrees(const T radians)
		{
			return radians * (T(180.0) / M_PI);
		}

		template<Arithmetic T>
		static constexpr auto lerp(const T a, const T b, const double f)
		{
			return (a * (1.0 - f)) + (b * f);
		}

		template<std::integral T>
		static constexpr auto Wrap(T val, const T min, const T max)
		{
			if (val > min && val < max)
				return val;

			const auto range_size = max - min;

			if (val < min)
				return max - ((min - val) % range_size);

			return min + ((val - min) % range_size);
		}
		template<std::floating_point T>
		static constexpr auto Wrap(T val, const T min, const T max)
		{
			if (val > min && val < max)
				return val;

			const auto range_size = max - min;

			if (val < min)
				return max - std::fmod(min - val, max - min);

			return min + std::fmod(val - min, max - min);
		}

		template<Arithmetic T>
		static constexpr auto MapToRange(const T val, const T minIn, const T maxIn, const T minOut, const T maxOut)
		{
			double x = (val - minIn) / (maxIn - minIn);
			return minOut + (maxOut - minOut) * x;
		}

		template<Arithmetic T>
		static constexpr auto SetPrecision(const T val, const int places)
		{
			double n = std::pow(10.0, (double)places);
			return std::round(val * n) / n;
		}
	}
}
