#pragma once

#include <concepts>
#include <filesystem>
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

	template<typename... Args>
	concept Exists = sizeof...(Args) > 0;

	template<std::size_t sz, typename... Args>
	concept SameSize = requires { sz == (sizeof(Args) + ... + 0); };

	template<class Resource>
	concept IsLoadable = requires(Resource&& resource)
	{
		{ resource.loadFromFile( std::filesystem::path() ) } -> std::same_as<bool>;
	};

	template<class Input, typename Bind>
	concept IsButtonInput = requires(Input&& input)
	{
		{ input.Pressed(Bind()) } -> std::same_as<bool>;
		{ input.Released(Bind()) } -> std::same_as<bool>;
		{ input.Held(Bind()) } -> std::same_as<bool>;
	};
}