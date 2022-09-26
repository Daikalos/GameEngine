#pragma once

#include <concepts>
#include <filesystem>
#include <type_traits>

namespace vlx // concepts is the best thing ever
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept Integral = std::is_integral_v<T>;

	template<typename T>
	concept Enum = std::is_enum_v<T>;

	template<typename T>
	concept ArithEnum = Arithmetic<T> || Enum<T>;

	template<typename T>
	concept IntEnum = Integral<T> || Enum<T>;

	template<typename T, typename... Args>
	concept IsSameType = std::conjunction_v<std::is_same<T, Args>...>;

	template<typename T>
	concept IsStringType = std::is_convertible_v<T, std::string_view>;

	template<typename... Args>
	concept Exists = sizeof...(Args) > 0;

	template<std::size_t sz, typename... Args>
	concept IsSameSize = requires { sz == (sizeof(Args) + ... + 0); };

	template<class Resource>
	concept IsLoadable = requires(Resource resource)
	{
		{ resource.loadFromFile( std::filesystem::path() ) } -> std::same_as<bool>;
	};

	template<class Input, typename Bind>
	concept IsButtonInput = requires(Input input)
	{
		{ input.Pressed(Bind()) } -> std::same_as<bool>;
		{ input.Released(Bind()) } -> std::same_as<bool>;
		{ input.Held(Bind()) } -> std::same_as<bool>;
	};

	template<class C>
	concept IsComponentType = requires 
	{ 
		std::is_class_v<C> && std::copyable<C> && sizeof(C) > 1; 
	};

	template<class T>
	concept IsVector = requires(T obj)
	{
		std::same_as<T, std::vector<typename T::value_type>>;
	};
}