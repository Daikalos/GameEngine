#pragma once

#include <concepts>
#include <type_traits>

namespace fge
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;
}