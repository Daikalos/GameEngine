#pragma once

#include <concepts>
#include <type_traits>

namespace vlx
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept Enum = std::is_enum_v<T>;

	template<typename T>
	concept ArithEnum = Arithmetic<T> || Enum<T>;

	template<typename T, typename... Args>
	concept SameType = std::conjunction_v<std::is_same<T, Args>...>;

	template<typename T>
	concept StringType = std::is_convertible_v<T, std::string_view>;

	template<typename Resource>
	concept IsLoadable = requires(Resource&& resource, std::filesystem::path&& path)
	{
		{ resource.loadFromFile(path) } -> std::same_as<bool>;
	};
}