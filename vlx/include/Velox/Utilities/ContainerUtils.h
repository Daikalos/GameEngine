#pragma once

#include <vector>
#include <array>

#include "Concepts.h"

namespace vlx::cu
{
	template<IsContainer T>
	struct ContainerHash
	{
		constexpr std::size_t operator()(const T& container) const
		{
			std::size_t seed = container.size();

			for (auto x : container)
			{
				x = ((x >> 16) ^ x) * 0x45d9f3b;
				x = ((x >> 16) ^ x) * 0x45d9f3b;
				x = (x >> 16) ^ x;

				seed ^= static_cast<std::size_t>(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};

	template<typename... Args> requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
	[[nodiscard]] static constexpr auto PackArray(Args&&... args)
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
	static constexpr void UnpackArray(const std::vector<std::byte>& data, Args&... args)
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
	static constexpr bool Erase(std::vector<T>& vector, const T& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return false;

		vector.erase(it);

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	static constexpr bool Erase(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end()) // do not erase if not found
			return false;

		vector.erase(it);

		return true;
	}

	template<typename T> 
	static constexpr bool SwapPop(std::vector<T>& vector, const T& item)
	{
		auto it = std::find(vector.begin(), vector.end(), item);

		if (it == vector.end())
			return false;
				
		*it = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<typename T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	static constexpr bool SwapPop(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end())
			return false;

		*it = std::move(vector.back());
		vector.pop_back();

		return true;
	}

	template<IsContainer T, class iter = typename T::const_iterator, typename U = typename T::value_type>
	[[nodiscard]] static constexpr iter FindSorted(const T& container, const U& item)
	{
		return std::lower_bound(container.begin(), container.end(), item);
	}

	template<IsContainer T>
	static constexpr bool InsertSorted(T& container, typename T::const_reference item)
	{
		const typename T::const_iterator it = FindSorted(container, item);

		if (it == container.cend() || *it != item)
		{
			container.insert(it, item);
			return true;
		}

		return false;
	}

	template<IsContainer T>
	static constexpr bool EraseSorted(T& container, typename T::const_reference item)
	{
		const typename T::const_iterator it = FindSorted(container, item);

		if (it != container.cend() && *it == item)
		{
			container.erase(it);
			return true;
		}

		return false;
	}

	template<IsContainer T>
	static constexpr void Sort(T& cntn)
	{
		std::sort(cntn.begin(), cntn.end());
	}

	template<IsContainer T>
	static constexpr T Sort(T&& cntn)
	{
		std::sort(cntn.begin(), cntn.end());
		return cntn;
	}

	template<IsContainer T, typename Comp>
	static constexpr T Sort(T&& cntn, Comp&& comp)
	{
		std::sort(cntn.begin(), cntn.end(), comp);
		return cntn;
	}

	template<IsContainer T>
	static constexpr bool IsSorted(T&& cntn)
	{
		return std::is_sorted(cntn.begin(), cntn.end());
	}

	template<IsContainer T>
	static constexpr bool IsSorted(const T& cntn)
	{
		return std::is_sorted(cntn.begin(), cntn.end());
	}

	template<IsContainer T, typename Comp>
	static constexpr bool IsSorted(T&& cntn, Comp&& comp)
	{
		return std::is_sorted(cntn.begin(), cntn.end(), comp);
	}

	template<IsContainer T, typename Comp>
	static constexpr bool IsSorted(const T& cntn, Comp&& comp)
	{
		return std::is_sorted(cntn.begin(), cntn.end(), comp);
	}
}