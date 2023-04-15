#pragma once

#include <Velox/System/Concepts.h>

namespace vlx::ma
{
    template<Arithmetic T>
    constexpr T Abs(T x) noexcept
    {
        return std::abs(x);
    }
}