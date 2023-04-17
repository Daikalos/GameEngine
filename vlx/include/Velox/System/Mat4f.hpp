#pragma once

#include <SFML/Graphics/Transform.hpp>

#include <Velox/Config.hpp>

#include "Rectangle.hpp"
#include "Vector2.hpp"

namespace vlx
{
	class Mat4f
	{
	public:
		constexpr Mat4f() = default;
		constexpr Mat4f(float a00, float a01, float a02,
						float a10, float a11, float a12,
						float a20, float a21, float a22);

		constexpr Mat4f(const sf::Transform& rhs);

	public:
		constexpr operator sf::Transform&();
		constexpr operator const sf::Transform&() const;

		NODISC constexpr bool operator==(const Mat4f& rhs);
		NODISC constexpr bool operator!=(const Mat4f& rhs);

		constexpr Mat4f operator*(const Mat4f& rhs) const;
		constexpr Mat4f& operator*=(const Mat4f& rhs);

		constexpr Vector2f operator*(const Vector2f& rhs) const;
		constexpr sf::Vector2f operator*(const sf::Vector2f& rhs) const;

	public:
		static constexpr Mat4f Lerp(const Mat4f& lhs, const Mat4f& rhs, float alpha);

	public:
		constexpr const float* GetMatrix() const noexcept;
		constexpr float Get(int x, int y) const;

		constexpr Mat4f GetInverse() const;
		constexpr Mat4f GetTranspose() const;

		constexpr Mat4f GetRotationMatrix() const;

		constexpr Vector2f TransformPoint(const Vector2f& point) const;
		constexpr sf::Vector2f TransformPoint(const sf::Vector2f& point) const;
		constexpr RectFloat TransformRect(const RectFloat& rect) const;

		constexpr Mat4f& Combine(const Mat4f& transform);
		constexpr Mat4f& Translate(const Vector2f& offset);
		constexpr Mat4f& Scale(const Vector2f& factor);
		constexpr Mat4f& Scale(const Vector2f& factor, const Vector2f& center);

		VELOX_API Mat4f& Rotate(sf::Angle angle);
		VELOX_API Mat4f& Rotate(sf::Angle angle, const Vector2f& center);

	public:
		static const Mat4f Identity;

	private:
		union
		{
			sf::Transform m_transform{};
			float m_matrix[16];
		};
	};

	constexpr Mat4f::Mat4f(float a00, float a01, float a02,
						   float a10, float a11, float a12,
						   float a20, float a21, float a22) 
		: m_transform(a00, a01, a02,
					  a10, a11, a12,
					  a20, a21, a22) {}

	constexpr Mat4f::Mat4f(const sf::Transform& rhs)
		: m_transform(rhs) {}

	constexpr Mat4f::operator sf::Transform&() 
	{
		return m_transform;
	}
	constexpr Mat4f::operator const sf::Transform&() const 
	{
		return m_transform;
	}

	constexpr bool Mat4f::operator==(const Mat4f& rhs) 
	{
		return m_transform == rhs.m_transform;
	}
	constexpr bool Mat4f::operator!=(const Mat4f& rhs) 
	{
		return m_transform != rhs.m_transform;
	}

	constexpr Mat4f Mat4f::operator*(const Mat4f& rhs)  const
	{
		return static_cast<Mat4f>(m_transform * rhs);
	}
	constexpr Mat4f& Mat4f::operator*=(const Mat4f& rhs) 
	{
		m_transform *= rhs;
		return *this;
	}

	constexpr Vector2f Mat4f::operator*(const Vector2f& rhs) const
	{
		return m_transform * rhs;
	}
	constexpr sf::Vector2f Mat4f::operator*(const sf::Vector2f& rhs) const
	{
		return m_transform * rhs;
	}

	constexpr Mat4f Mat4f::Lerp(const Mat4f& lhs, const Mat4f& rhs, float alpha)
	{
		Mat4f rot = lhs.GetRotationMatrix();

		sf::Angle angle;

		return Mat4f();
	}

	constexpr const float* Mat4f::GetMatrix() const noexcept 
	{
		return m_matrix;
	}
	constexpr float Mat4f::Get(int x, int y) const
	{
		return m_matrix[x + y * 4];
	}

	constexpr Mat4f Mat4f::GetInverse() const 
	{
		return static_cast<Mat4f>(m_transform.getInverse());
	}
	constexpr Mat4f Mat4f::GetTranspose() const 
	{
		return Mat4f(m_matrix[0],	m_matrix[1],	m_matrix[3],
					 m_matrix[4],	m_matrix[5],	m_matrix[7],
					 m_matrix[12],	m_matrix[13],	m_matrix[15]);
	}

	constexpr Mat4f Mat4f::GetRotationMatrix() const
	{
		return Mat4f(m_matrix[0],	m_matrix[4],	0.0f,
					 m_matrix[1],	m_matrix[5],	0.0f,
					 0.0f,			0.0f,			1.0f);
	}

	constexpr Vector2f Mat4f::TransformPoint(const Vector2f& point) const 
	{
		return m_transform.transformPoint(point);
	}
	constexpr sf::Vector2f Mat4f::TransformPoint(const sf::Vector2f& point) const
	{
		return m_transform.transformPoint(point);
	}
	constexpr RectFloat Mat4f::TransformRect(const RectFloat& rect) const 
	{
		return m_transform.transformRect(rect);
	}

	constexpr Mat4f& Mat4f::Combine(const Mat4f& transform) 
	{
		m_transform.combine(transform);
		return *this;
	}
	constexpr Mat4f& Mat4f::Translate(const Vector2f& offset) 
	{
		m_transform.translate(offset);
		return *this;
	}
	constexpr Mat4f& Mat4f::Scale(const Vector2f& factor) 
	{
		m_transform.scale(factor);
		return *this;
	}
	constexpr Mat4f& Mat4f::Scale(const Vector2f& factor, const Vector2f& center) 
	{
		m_transform.scale(factor, center);
		return *this;
	}

	inline constexpr Mat4f Mat4f::Identity;
}