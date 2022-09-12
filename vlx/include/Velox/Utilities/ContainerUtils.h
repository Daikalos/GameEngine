#pragma once

#include <vector>
#include <array>

#include "Concepts.h"

namespace vlx::cu
{
	template<class T, std::equality_comparable_with<T> U>
	[[nodiscard]] static constexpr auto Erase(std::vector<T>& vector, const U& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return it;

		vector.erase(it);

		return it;
	}

	template<class T, class Func>
	[[nodiscard]] static constexpr auto Erase(std::vector<T>& vector, Func&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), pred);

		if (it == vector.end()) // do not erase if not found
			return it;

		vector.erase(it);

		return it;
	}

	template<class T, std::equality_comparable_with<T> U>
	[[nodiscard]] static constexpr auto SwapPop(std::vector<T>& vector, const U& compare)
	{
		auto it = std::find(vector.begin(), vector.end(), compare);

		if (it == vector.end())
			return it;
				
		std::iter_swap(it, vector.end() - 1);
		vector.pop_back();

		return it;
	}

	template<class T, class Func>
	[[nodiscard]] static constexpr auto SwapPop(std::vector<T>& vector, Func&& pred)
	{
		auto it = std::find_if(vector.begin(), vector.end(), pred);

		if (it == vector.end())
			return it;

		std::iter_swap(it, vector.end() - 1);
		vector.pop_back();

		return it;
	}

	template<typename... Args>
	[[nodiscard]] static constexpr auto PackArray(Args&&... args) requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
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

	template<typename... Args>
	static constexpr void UnpackArray(const std::vector<std::byte>& data, Args&... args) requires (std::is_trivially_copyable_v<std::remove_reference_t<Args>> && ...)
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
}