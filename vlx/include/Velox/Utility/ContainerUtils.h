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
		return std::all_of(begin, end, std::forward<Func>(func));
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

		if (i == vector.size() - 1) // if popping last, no need to move item
		{
			vector.pop_back();
			return true;
		}

		vector[i] = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<typename T>
	inline auto InsertSorted(std::vector<T>& v, const T& item)
	{
		return v.insert(std::upper_bound(v.begin(), v.end(), item), item);
	}

	template<typename T, typename Comp>
	inline auto InsertSorted(std::vector<T>& v, const T& item, Comp&& comparison)
	{
		return v.insert(std::upper_bound(v.begin(), v.end(), item, std::forward<Comp>(comparison)), item);
	}

	template<typename T>
	inline bool EraseSorted(std::vector<T>& v, const T& item)
	{
		const auto lb = std::lower_bound(v.begin(), v.end(), item);

		if (lb != v.cend() && *lb == item)
		{
			const auto ub = std::upper_bound(lb, v.end(), item);
			v.erase(lb, ub); // remove all equal elements in range

			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool EraseSorted(std::vector<T>& v, const T& item, Comp&& comparison)
	{
		const auto lb = std::lower_bound(v.begin(), v.end(), item, std::forward<Comp>(comparison));

		if (lb != v.cend() && *lb == item)
		{
			const auto ub = std::upper_bound(lb, v.end(), item, std::forward<Comp>(comparison));
			v.erase(lb, ub); // remove all equal elements in range

			return true;
		}

		return false;
	}

	template<typename T>
	inline bool InsertUniqueSorted(std::vector<T>& v, const T& item)
	{
		const auto it = std::upper_bound(v.begin(), v.end(), item);

		if (it == v.cend() || *it != item)
		{
			v.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool InsertUniqueSorted(std::vector<T>& v, const T& item, Comp&& comparison)
	{
		const auto it = std::upper_bound(v.begin(), v.end(), item, std::forward<Comp>(comparison));

		if (it == v.cend() || *it != item)
		{
			v.insert(it, item);
			return true;
		}

		return false;
	}

	template<typename T>
	inline bool EraseUniqueSorted(std::vector<T>& v, const T& item)
	{
		const auto it = std::lower_bound(v.begin(), v.end(), item);

		if (it != v.cend() && *it == item)
		{
			v.erase(it);
			return true;
		}

		return false;
	}

	template<typename T, typename Comp>
	inline bool EraseUniqueSorted(std::vector<T>& v, const T& item, Comp&& comparison)
	{
		const auto it = std::lower_bound(v.begin(), v.end(), item, std::forward<Comp>(comparison));

		if (it != v.cend() && *it == item)
		{
			v.erase(it);
			return true;
		}

		return false;
	}

	template<typename T>
	inline constexpr void Sort(std::span<const T> items)
	{
		std::sort(items.begin(), items.end());
	}

	template<typename T>
	NODISC inline constexpr T& Sort(T& items)
	{
		std::sort(std::begin(items), std::end(items));
		return items;
	}

	template<typename T, typename Comp>
	NODISC inline constexpr T& Sort(T& items, Comp&& comp)
	{
		std::sort(std::begin(items), std::end(items), std::forward<Comp>(comp));
		return items;
	}

	template<typename T>
	NODISC inline constexpr T Sort(T&& items)
	{
		std::sort(std::begin(items), std::end(items));
		return items;
	}

	template<typename T, typename Comp>
	NODISC inline constexpr T Sort(T&& items, Comp&& comp)
	{
		std::sort(std::begin(items), std::end(items), std::forward<Comp>(comp));
		return items;
	}

	template<typename T>
	NODISC inline constexpr bool IsSorted(std::span<const T> items)
	{
		return std::is_sorted(items.begin(), items.end());
	}

	template<typename T, typename Comp>
	NODISC inline constexpr bool IsSorted(std::span<const T> items, Comp&& comp)
	{
		return std::is_sorted(items.begin(), items.end(), comp);
	}

	template<typename T>
	inline constexpr void HashCombine(uint64& seed, const T& v)
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