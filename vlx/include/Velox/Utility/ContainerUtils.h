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
			constexpr auto size = sizeof(arg);

			std::memcpy(data.data() + offset, &arg, size);
			offset += size;
		};

		(pack(args), ...);

		return data;
	}

	template<typename... Args> requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
	inline void UnpackArray(const std::vector<std::byte>& data, Args&... args)
	{
		if (data.size() != (sizeof(Args) + ... + 0))
			throw std::runtime_error("not the same size");

		std::size_t offset = 0;

		const auto unpack = [&data, &offset](auto& arg)
		{
			constexpr auto size = sizeof(arg);

			std::memcpy(&arg, data.data() + offset, size);
			offset += size;
		};

		(unpack(args), ...);
	}

	template<typename It, typename Func>
	inline auto ForEachUntil(It begin, It end, Func&& func)
	{
		return std::ranges::all_of(begin, end, std::forward<Func>(func));
	}

	template<typename T>
	inline bool Erase(std::vector<T>& vec, const T& compare)
	{
		auto it = std::ranges::find(vec, compare);

		if (it == vec.end())
			return false;

		vec.erase(it);

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	inline bool Erase(std::vector<T>& vec, Pred&& pred)
	{
		auto it = std::ranges::find_if(vec, std::forward<Pred>(pred));

		if (it == vec.end()) // do not erase if not found
			return false;

		vec.erase(it);

		return true;
	}

	template<typename T> 
	inline bool SwapPop(std::vector<T>& vec, const T& item)
	{
		auto it = std::ranges::find(vec, item);

		if (it == vec.end())
			return false;
				
		*it = std::move(vec.back());
		vec.pop_back();

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	inline bool SwapPop(std::vector<T>& vec, Pred&& pred)
	{
		auto it = std::ranges::find_if(vec, std::forward<Pred>(pred));

		if (it == vec.end())
			return false;

		*it = std::move(vec.back());
		vec.pop_back();

		return true;
	}

	template<typename T>
	inline bool SwapPopAt(std::vector<T>& vec, std::size_t i)
	{
		if (vec.empty() || i >= vec.size())
			return false;

		if (i == vec.size() - 1) // if popping last, no need to move item
		{
			vec.pop_back();
			return true;
		}

		vec[i] = std::move(vec.back());
		vec.pop_back();

		return true;
	}

	template<typename T>
	inline auto InsertSorted(std::vector<T>& vec, const T& item)
	{
		return vec.insert(std::ranges::upper_bound(vec, item), item);
	}

	template<typename T, typename Comp>
	inline auto InsertSorted(std::vector<T>& vec, const T& item, Comp&& comparison)
	{
		return vec.insert(std::ranges::upper_bound(vec, item, std::forward<Comp>(comparison)), item);
	}

	template<typename T>
	inline bool EraseSorted(std::vector<T>& vec, const T& item)
	{
		const auto lb = std::ranges::lower_bound(vec, item);

		if (lb != vec.cend() && *lb == item)
		{
			const auto ub = std::upper_bound(lb, vec.end(), item);
			vec.erase(lb, ub); // remove all equal elements in range

			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool EraseSorted(std::vector<T>& vec, const T& item, Comp&& comparison)
	{
		const auto lb = std::ranges::lower_bound(vec, item, std::forward<Comp>(comparison));

		if (lb != vec.cend() && *lb == item)
		{
			const auto ub = std::ranges::upper_bound(lb, vec.end(), item, std::forward<Comp>(comparison));
			vec.erase(lb, ub); // remove all equal elements in range

			return true;
		}

		return false;
	}

	template<typename T>
	inline bool InsertUniqueSorted(std::vector<T>& vec, const T& item)
	{
		const auto it = std::ranges::upper_bound(vec, item);

		if (it == vec.cend() || *it != item)
		{
			vec.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool InsertUniqueSorted(std::vector<T>& vec, const T& item, Comp&& comparison)
	{
		const auto it = std::ranges::upper_bound(vec, item, std::forward<Comp>(comparison));

		if (it == vec.cend() || *it != item)
		{
			vec.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T>
	inline bool EraseUniqueSorted(std::vector<T>& vec, const T& item)
	{
		const auto it = std::ranges::lower_bound(vec, item);

		if (it != vec.cend() && *it == item)
		{
			vec.erase(it);
			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool EraseUniqueSorted(std::vector<T>& vec, const T& item, Comp&& comparison)
	{
		const auto it = std::ranges::lower_bound(vec, item, std::forward<Comp>(comparison));

		if (it != vec.cend() && *it == item)
		{
			vec.erase(it);
			return true;
		}

		return false;
	}

	template<typename T>
	constexpr void Sort(std::span<const T> items)
	{
		std::ranges::sort(items);
	}

	template<typename T>
	NODISC constexpr T& Sort(T& items)
	{
		std::ranges::sort(items);
		return items;
	}

	template<typename T, typename Comp>
	NODISC constexpr T& Sort(T& items, Comp&& comp)
	{
		std::ranges::sort(items, std::forward<Comp>(comp));
		return items;
	}

	template<typename T>
	NODISC constexpr T Sort(T&& items)
	{
		std::ranges::sort(items);
		return items;
	}

	template<typename T, typename Comp>
	NODISC constexpr T Sort(T&& items, Comp&& comp)
	{
		std::ranges::sort(items, std::forward<Comp>(comp));
		return items;
	}

	template<typename T>
	NODISC constexpr bool IsSorted(std::span<const T> items)
	{
		return std::ranges::is_sorted(items);
	}

	template<typename T, typename Comp>
	NODISC constexpr bool IsSorted(std::span<const T> items, Comp&& comp)
	{
		return std::ranges::is_sorted(items, std::forward<Comp>(comp));
	}

	template<typename T, typename U, typename V>
	NODISC constexpr V Merge(const T& r1, const U& r2)
	{
		V result;
		std::ranges::set_union(r1, r2, std::begin(result));
		return result;
	}

	template<typename T>
	constexpr void HashCombine(uint64& seed, const T& v)
	{
		seed ^= static_cast<uint64>(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename T>
	struct ContainerHash
	{
		NODISC constexpr uint64 operator()(std::span<const T> items) const
		{
			uint64 seed = items.size();

			if (seed == 1)
				return items.front(); // just return first if only one

			for (auto x : items)
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