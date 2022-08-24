#pragma once

#define _USE_MATH_DEFINES

#include <FGEConcepts.hpp>

#include <math.h>
#include <random>
#include <chrono>

namespace fge
{
	////////////////////////////////////////////////////////////
	// General utility class
	////////////////////////////////////////////////////////////
	namespace util
	{
		template<typename T>
		static constexpr T ToRadians(const T degrees) requires Arithmetic<T>
		{
			return T(degrees * (float(M_PI) / 180.0f));
		}

		template<typename T>
		static constexpr T ToDegrees(const T radians) requires Arithmetic<T>
		{
			return T(radians * (180.0f / float(M_PI)));
		}

		template<typename T>
		static constexpr T lerp(const T a, const T b, const double f) requires Arithmetic<T>
		{
			return (a * (1.0f - f)) + (b * f);
		}

		template<typename T>
		static constexpr T Wrap(T val, const T min, const T max) requires std::integral<T>
		{
			if (val > min && val < max)
				return val;

			const auto range_size = max - min;

			if (val < min)
				return max - ((min - val) % range_size);

			return min + ((val - min) % range_size);
		}
		template<typename T>
		static constexpr T Wrap(T val, const T min, const T max) requires std::floating_point<T>
		{
			if (val > min && val < max)
				return val;

			const auto range_size = max - min;

			if (val < min)
				return max - std::fmod(min - val, max - min);

			return min + std::fmod(val - min, max - min);
		}

		template<typename T>
		static constexpr T MapToRange(const T val, const T minIn, const T maxIn, const T minOut, const T maxOut) requires Arithmetic<T>
		{
			double x = (val - minIn) / (maxIn - minIn);
			return minOut + (maxOut - minOut) * x;
		}

		template<typename T>
		static constexpr T SetPrecision(const T val, const int places) requires Arithmetic<T>
		{
			double n = std::pow(10.0, (double)places);
			return std::roundf(val * n) / n;
		}
	}
}
