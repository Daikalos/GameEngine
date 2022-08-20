#pragma once

#include <type_traits>
#include <random>
#include <chrono>

namespace fge
{
	namespace rnd
	{
		static thread_local std::mt19937_64 dre(std::chrono::steady_clock::now().time_since_epoch().count());

		template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		static inline T random(const T min, const T max)
		{
			std::uniform_real_distribution<T> uid(min, max);
			return (T)uid(dre);
		}
		template<typename T, typename std::enable_if_t<!std::is_floating_point_v<T>, bool> = true>
		static inline T random(const T min, const T max)
		{
			std::uniform_int_distribution<T> uid(min, max);
			return (T)uid(dre);
		}

		template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
		static inline std::vector<T> random(const T size)
		{
			std::vector<T> result;
			result.reserve(size);

			for (T i = 0; i < size; ++i)
				result.push_back(i);

			std::shuffle(result.begin(), result.end(), dre);

			return result;
		}

		template<typename T, typename... Args, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
		static inline T random_arg(Args&&... args)
		{
			std::vector<T> x{ { std::forward<Args>(args)... } };
			return x[util::random<size_t>(0, x.size() - 1)];
		}
	}
}