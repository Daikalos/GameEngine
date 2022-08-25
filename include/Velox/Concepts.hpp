#pragma once

#include <concepts>
#include <type_traits>

namespace vlx
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept Enum = std::is_enum_v<T>;
}