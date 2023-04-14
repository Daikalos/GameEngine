#pragma once

#include <Velox/System/Concepts.h>

namespace vlx::ma
{
    template<Arithmetic T>
    constexpr T Abs(T x) noexcept
    {
        return std::abs(x);
    }

    constexpr bool BiasGreaterThan(float a, float b)
    {
        constexpr float k_bias_relative = 0.95f;
        constexpr float k_bias_absolute = 0.01f;

        return a >= b * k_bias_relative + a * k_bias_absolute;
    }
}