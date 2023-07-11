#pragma once

#include <concepts>
#include <filesystem>

#include <Velox/Types.hpp>

#include "Traits.h"

namespace vlx // concepts is the best thing ever
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept Integral = std::is_integral_v<T>;

	template<typename T>
	concept FloatingPoint = std::is_floating_point_v<T>;

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

	template<typename T, typename... Args>
	concept IsPresent = (std::same_as<T, Args> || ...);

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

	template<class Lambda, typename T, std::size_t... Index>
	concept SameTypeParamDecay = (std::same_as<T, std::decay_t<typename traits::FunctionTraits<Lambda>::template arg_type<Index>>> && ...);

	template<class Lambda, typename T, std::size_t... Index>
	concept SameTypeParam = (std::same_as<T, typename traits::FunctionTraits<Lambda>::template arg_type<Index>> && ...);

	template<class T, class... Args>
	concept Contains = std::disjunction_v<std::is_same<T, Args>...>;
}