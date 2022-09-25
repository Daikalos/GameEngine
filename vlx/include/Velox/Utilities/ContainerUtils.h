#pragma once

#include <vector>
#include <array>

#include "Concepts.h"

namespace vlx::cu
{
	template<class T>
	[[nodiscard]] static constexpr bool Erase(std::vector<T>& vector, const T& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return false;

		vector.erase(it);

		return true;
	}

	template<class T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	[[nodiscard]] static constexpr bool Erase(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end()) // do not erase if not found
			return false;

		vector.erase(it);

		return true;
	}

	template<class T>
	[[nodiscard]] static constexpr bool SwapPop(std::vector<T>& vector, const T& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return false;
				
		*it = vector.back();
		vector.pop_back();

		return true;
	}

	template<class T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	[[nodiscard]] static constexpr bool SwapPop(std::vector<T>& vector, Pred&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), std::forward<Pred>(pred));

		if (it == vector.end())
			return false;

		*it = vector.back();
		vector.pop_back();

		return true;
	}

	template<class T>
	[[nodiscard]] static constexpr auto FindSorted(const std::vector<T>& vector, const T& item)
	{
		return std::lower_bound(vector.begin(), vector.end(), item);
	}

	template<class T>
	[[nodiscard]] static constexpr auto InsertSorted(std::vector<T>& vector, const T& item)
	{
		return vector.insert(std::upper_bound(
			vector.begin(), vector.end(), item), item);
	}

	template<class T>
	[[nodiscard]] static constexpr bool InsertUniqueSorted(std::vector<T>& vector, const T& item)
	{
		for (auto it = vector.begin(); it != vector.end(); ++it) // custom algorithm for inserting an item in a sorted list containing only unique values
		{
			if (*it == item)
				return false;

			if (*it > item)
			{
				vector.insert(it, item);
				return true;
			}
		}

		vector.insert(vector.end(), item);

		return true;
	}

	template<class T, class Pred> requires (!std::equality_comparable_with<T, Pred>)
	[[nodiscard]] static constexpr auto InsertSorted(std::vector<T>& vector, const T& item, Pred&& pred)
	{
		return vector.insert(std::upper_bound(
				vector.begin(), vector.end(), item, std::forward<Pred>(pred)), item);
	}

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

	template<IsVector T>
	struct VectorHash
	{
		typename T::value_type operator()(const T& container) const
		{
			typename T::value_type hash = container.size();

			for (auto& i : container)
				hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			return hash;
		}
	};
}