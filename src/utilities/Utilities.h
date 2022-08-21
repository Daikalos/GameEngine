#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <random>
#include <chrono>

namespace fge
{
	namespace util
	{
		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T ToRadians(const T degrees)
		{
			return T(degrees * (float(M_PI) / 180.0f));
		}

		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T ToDegrees(const T radians)
		{
			return T(radians * (180.0f / float(M_PI)));
		}

		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T lerp(const T a, const T b, const float f)
		{
			return (a * (1.0f - f)) + (b * f);
		}

		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T Clamp(const T val, const T min, const T max)
		{
			if (val < min)
				return min;
			if (val > max)
				return max;

			return val;
		}

		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T Wrap(T val, const T min, const T max)
		{
			if (val > min && val < max)
				return val;

			const T range_size = max - min;

			if (val < min)
				return max - std::fmodf(min - val, range_size);

			return min + std::fmodf(val - min, range_size);
		}

		template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static constexpr T MapToRange(const T val, const T minIn, const T maxIn, const T minOut, const T maxOut)
		{
			float x = (val - minIn) / (maxIn - minIn);
			return minOut + (maxOut - minOut) * x;
		}

		template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		static constexpr T SetPrecision(const T val, const int places)
		{
			float n = std::powf(10.0f, (float)places);
			return std::roundf(val * n) / n;
		}

		static constexpr std::string RemoveTrailingZeroes(const std::string& str)
		{
			std::string result = str;

			if (result.find('.') != std::string::npos)
			{
				result = result.substr(0, result.find_last_not_of('0') + 1);

				if (result.find('.') == result.size() - 1)
					result = result.substr(0, result.size() - 1);
			}

			return result;
		}
	}
}
