#pragma once

#include <SFML/Graphics/Transform.hpp>

#include <Velox/Config.hpp>

#include "Rectangle.hpp"
#include "Vector2.hpp"

namespace vlx
{
	class Mat2f
	{
	public:
		constexpr Mat2f() = default;
		constexpr Mat2f(float a00, float a01,
			float a10, float a11);

		Mat2f(float radians);

	public:
		constexpr Mat2f GetInverse() const;

	public:
		constexpr Mat2f Transpose() const;

		constexpr Mat2f Abs() const;

		constexpr Mat2f AxisX() const;
		constexpr Mat2f AxisY() const;

	private:
		float M[4]{ 1.f, 0.f,
					0.f, 1.f };
	};
}