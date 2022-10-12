#pragma once

#include <concepts>
#include <filesystem>

#include "Traits.h"

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
	concept IsSameSize = requires 
	{
		sz == (sizeof(Args) + ... + 0); 
	};

	template<class T>
	concept IsVector = requires
	{
		std::same_as<T, std::vector<typename T::value_type>>;
	};

	template<typename... Ts>
	concept NoDuplicates = requires
	{
		traits::NoDuplicates<Ts...>{};
	};

	template<class Lambda, typename T, std::size_t Index = 0>
	concept SameTypeParameter = requires
	{
		std::same_as<T, typename traits::FunctionTraits<Lambda>::template arg_type<Index>>;
	};

	template<class Resource>
	concept IsLoadable = requires(Resource resource)
	{
		{ resource.loadFromFile(std::filesystem::path()) } -> std::same_as<bool>;
	};

	template<class Input, typename Bind>
	concept IsButtonInput = requires(Input input)
	{
		{ input.Pressed(Bind()) } -> std::same_as<bool>;
		{ input.Released(Bind()) } -> std::same_as<bool>;
		{ input.Held(Bind()) } -> std::same_as<bool>;
	};

	template<class C>
	concept IsComponent = std::is_class_v<C> && std::copyable<C> && sizeof(C) > 1;

	template<class... Cs>
	concept IsComponents = IsComponent<Cs...> && Exists<Cs...> && NoDuplicates<Cs...>;
}