#pragma once

#include <Velox/Utility/MathUtility.h>
#include <Velox/Config.hpp>

#include "Rectangle.hpp"
#include "Vector2.hpp"

namespace vlx
{
	class Mat2f
	{
	public:
		constexpr Mat2f();
		constexpr Mat2f(float a00, float a01,
						float a10, float a11);

		VELOX_API Mat2f(sf::Angle angle);

	public:
		constexpr Mat2f operator*(const Mat2f& rhs) const;
		constexpr Mat2f& operator*=(const Mat2f& rhs);

		constexpr Vector2f operator*(const Vector2f& rhs) const;

	public:
		VELOX_API void Set(sf::Angle angle);

	public:
		constexpr Mat2f GetInverse() const;
		constexpr Mat2f GetTranspose() const;

		constexpr Mat2f& Combine(const Mat2f& transform);

		constexpr Mat2f GetAbs() const;

		constexpr Vector2f GetAxisX() const;
		constexpr Vector2f GetAxisY() const;

	public:
		static const Mat2f Identity;

	private:
		float m_matrix[4]{ 1.f, 0.f,
							0.f, 1.f };
	};

	constexpr Mat2f::Mat2f() = default;

	constexpr Mat2f::Mat2f(float a00, float a01,
						   float a10, float a11) 
		: m_matrix{a00, a10, 
				   a01, a11} {}

	constexpr Mat2f Mat2f::operator*(const Mat2f& rhs) const 
	{
		return Mat2f(*this).Combine(rhs);
	}
	constexpr Mat2f& Mat2f::operator*=(const Mat2f& rhs) 
	{
		return Combine(rhs);
	}

	constexpr Vector2f Mat2f::operator*(const Vector2f& rhs) const 
	{
		return Vector2f(m_matrix[0] * rhs.x + m_matrix[2] * rhs.y, 
						m_matrix[1] * rhs.x + m_matrix[3] * rhs.y);
	}

	constexpr Mat2f Mat2f::GetInverse() const 
	{
		float det = m_matrix[0] * m_matrix[3] -
					m_matrix[2] * m_matrix[1];

		if (det != 0.0f)
		{
			det = 1.0f / det;

			return Mat2f( m_matrix[3] * det, 
						 -m_matrix[2] * det,
						 -m_matrix[1] * det,
						  m_matrix[0] * det);
		}
		
		return Identity;
	}
	constexpr Mat2f Mat2f::GetTranspose() const 
	{
		return Mat2f(m_matrix[0], m_matrix[1], 
					 m_matrix[2], m_matrix[3]);
	}

	constexpr Mat2f& Mat2f::Combine(const Mat2f& transform)
	{
		*this = Mat2f(m_matrix[0] * transform.m_matrix[0] + m_matrix[2] * transform.m_matrix[1],
					  m_matrix[0] * transform.m_matrix[2] + m_matrix[2] * transform.m_matrix[3],
					  m_matrix[1] * transform.m_matrix[0] + m_matrix[3] * transform.m_matrix[1],
					  m_matrix[1] * transform.m_matrix[2] + m_matrix[3] * transform.m_matrix[3]);

		return *this;
	}

	constexpr Mat2f Mat2f::GetAbs() const 
	{
		return Mat2f(ma::Abs(m_matrix[0]), ma::Abs(m_matrix[2]),
					 ma::Abs(m_matrix[1]), ma::Abs(m_matrix[3]));
	}

	constexpr Vector2f Mat2f::GetAxisX() const 
	{
		return Vector2f(m_matrix[0], m_matrix[1]);
	}
	constexpr Vector2f Mat2f::GetAxisY() const 
	{
		return Vector2f(m_matrix[2], m_matrix[3]);
	}

	inline constexpr Mat2f Mat2f::Identity;
}