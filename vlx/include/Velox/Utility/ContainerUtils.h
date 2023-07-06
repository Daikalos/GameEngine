#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <span>

#include <Velox/System/Concepts.h>
#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx::cu
{
	template<typename... Args> requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
	NODISC inline auto PackArray(Args&&... args)
	{
		std::vector<std::byte> data;
		data.resize((sizeof(Args) + ... + 0));

		std::size_t offset = 0;

		const auto pack = [&data, &offset](const auto& arg)
		{
			const auto& arg_size = sizeof(arg);

			std::memcpy(data.data() + offset, &arg, arg_size);
			offset += arg_size;
		};

		(pack(args), ...);

		return data;
	}

	template<typename... Args> requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
	NODISC inline void UnpackArray(const std::vector<std::byte>& data, Args&... args)
	{
		if (data.size() != (sizeof(Args) + ... + 0))
			throw std::runtime_error("not the same size");

		std::size_t offset = 0;

		const auto unpack = [&data, &offset](auto& arg)
		{
			const auto& arg_size = sizeof(arg);

			std::memcpy(&arg, data.data() + offset, arg_size);
			offset += arg_size;
		};

		(unpack(args), ...);
	}

	template<typename T>
	inline bool Erase(std::vector<T>& vector, const T& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return false;

		vector.erase(it);

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	inline bool Erase(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end()) // do not erase if not found
			return false;

		vector.erase(it);

		return true;
	}

	template<typename T> 
	inline bool SwapPop(std::vector<T>& vector, const T& item)
	{
		auto it = std::find(vector.begin(), vector.end(), item);

		if (it == vector.end())
			return false;
				
		*it = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	inline bool SwapPop(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end())
			return false;

		*it = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<typename T>
	inline bool SwapPopAt(std::vector<T>& vector, std::size_t i)
	{
		if (vector.empty() || i >= vector.size())
			return false;

		vector[i] = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<typename Iter, typename T>
	NODISC inline constexpr auto FindSorted(Iter begin, Iter end, const T& item)
	{
		return std::lower_bound(begin, end, item);
	}

	template<typename Iter, typename T, class Comp>
	NODISC inline constexpr auto FindSorted(Iter begin, Iter end, const T& item, Comp&& comparison)
	{
		return std::lower_bound(begin, end, item, std::forward<Comp>(comparison));
	}

	template<typename T>
	inline bool InsertSorted(std::vector<T>& container, const T& item)
	{
		const auto it = FindSorted(container.begin(), container.end(), item);

		if (it == container.cend() || *it != item)
		{
			container.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool InsertSorted(std::vector<T>& container, const T& item, Comp&& comparison)
	{
		const auto it = FindSorted(container.begin(), container.end(), item, std::forward<Comp>(comparison));

		if (it == container.cend() || *it != item)
		{
			container.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T>
	inline bool EraseSorted(std::vector<T>& container, const T& item)
	{
		const auto it = FindSorted(container.begin(), container.end(), item);

		if (it != container.cend() && *it == item)
		{
			container.erase(it);
			return true;
		}

		return false;
	}

	template<typename T>
	inline constexpr void Sort(std::span<const T> cntn)
	{
		std::sort(cntn.begin(), cntn.end());
	}

	template<typename T>
	NODISC inline constexpr T& Sort(T& cntn)
	{
		std::sort(std::begin(cntn), std::end(cntn));
		return cntn;
	}

	template<typename T, typename Comp>
	NODISC inline constexpr T& Sort(T& cntn, Comp&& comp)
	{
		std::sort(std::begin(cntn), std::end(cntn), std::forward<Comp>(comp));
		return cntn;
	}

	template<typename T>
	NODISC inline constexpr T Sort(T&& cntn)
	{
		std::sort(std::begin(cntn), std::end(cntn));
		return cntn;
	}

	template<typename T, typename Comp>
	NODISC inline constexpr T Sort(T&& cntn, Comp&& comp)
	{
		std::sort(std::begin(cntn), std::end(cntn), std::forward<Comp>(comp));
		return cntn;
	}

	template<typename T>
	NODISC inline constexpr bool IsSorted(std::span<const T> cntn)
	{
		return std::is_sorted(cntn.begin(), cntn.end());
	}

	template<typename T, typename Comp>
	NODISC inline constexpr bool IsSorted(std::span<const T> cntn, Comp&& comp)
	{
		return std::is_sorted(cntn.begin(), cntn.end(), comp);
	}

	template<typename T>
	NODISC inline constexpr void HashCombine(uint64& seed, const T& v)
	{
		seed ^= static_cast<uint64>(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename T>
	struct ContainerHash
	{
		NODISC constexpr uint64 operator()(std::span<const T> container) const
		{
			uint64 seed = container.size();

			if (seed == 1)
				return container.front(); // just return first if only one

			for (auto x : container)
			{
				x = ((x >> 16) ^ x) * 0x45d9f3b;
				x = ((x >> 16) ^ x) * 0x45d9f3b;
				x = (x >> 16) ^ x;

				HashCombine(seed, x);
			}

			return seed;
		}
	};

	template<std::integral T>
	struct PairIntegerHash
	{
		NODISC constexpr uint64 operator()(const std::pair<T, T>& pair) const
		{
			uint64 seed = 0;

			HashCombine(seed, pair.first);
			HashCombine(seed, pair.second);

			return seed;
		}
	};
}