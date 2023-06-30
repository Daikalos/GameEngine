#pragma once

#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>

#include <Velox/Config.hpp>

#include "Rectangle.hpp"
#include "Vector2.hpp"
#include "Mat2f.hpp"

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
		/// Retrieves the pointer to the matrix.
		/// 
		constexpr const float* GetMatrix() const noexcept;

		/// Retrieves value at (x, y) in the matrix.
		/// 
		constexpr float Get(int x, int y) const;

		/// Retrieves the translation of the matrix.
		/// 
		constexpr Vector2f GetTranslation() const;

		/// Retrieves the rotation of the matrix.
		/// 
		VELOX_API sf::Angle GetRotation() const;

		/// Retrieves the scale of the matrix.
		/// 
		constexpr Vector2f GetScale() const;

		/// Computes from this and returns an inversed matrix.
		/// 
		constexpr Mat4f GetInverse() const;

		/// Computes from this and returns a transposed matrix.
		/// 
		constexpr Mat4f GetTranspose() const;

		/// Returns a copy of this without the translation.
		/// 
		constexpr Mat2f GetRotationMatrix() const;

		/// Transforms the point into local space of this matrix.
		/// 
		/// \param Point: Point to transform
		/// 
		/// \returns Transformed point
		/// 
		constexpr Vector2f TransformPoint(const Vector2f& point) const;

		/// Transforms the point into local space of this matrix.
		/// 
		/// \param Point: Point to transform
		/// 
		/// \returns Transformed point
		/// 
		constexpr sf::Vector2f TransformPoint(const sf::Vector2f& point) const;

		/// Transforms the rect into local space of this matrix. Since the rect cannot be
		/// rotated, it will instead return a bounding rectangle of the transformed rectangle.
		/// 
		/// \param Rect: rect to transform
		/// 
		/// \returns Transformed rect
		/// 
		constexpr RectFloat TransformRect(const RectFloat& rect) const;

		/// Combines this matrix with another.
		/// 
		/// \param Transform: transform to combine with this
		/// 
		/// \returns Reference to *this
		/// 
		constexpr Mat4f& Combine(const Mat4f& transform);

		/// Combines this matrix with a translation.
		/// 
		/// \param Offset: translation offset to apply
		/// 
		/// \returns Reference to *this
		/// 
		constexpr Mat4f& Translate(const Vector2f& offset);

		/// Combines this matrix with a scaling.
		/// 
		/// \param Factors: scaling factors to apply
		/// 
		/// \returns Reference to *this
		/// 
		constexpr Mat4f& Scale(const Vector2f& factors);

		/// Combines this matrix with a scaling.
		/// 
		/// \param Factors: scaling factors to apply
		/// \param Center: center of scaling
		/// 
		/// \returns Reference to *this
		/// 
		constexpr Mat4f& Scale(const Vector2f& factors, const Vector2f& center);

		/// Builds a transform from the arguments.
		/// 
		/// \param Position: translation of matrix
		/// 
		/// \returns Reference to *this
		/// 
		constexpr Mat4f& Build(
			const Vector2f& position);

		/// Combine this matrix with a rotation.
		/// 
		/// \param Angle: rotation angle
		/// 
		/// \returns Reference to *this
		/// 
		VELOX_API Mat4f& Rotate(sf::Angle angle);

		/// Combine this matrix with a rotation.
		/// 
		/// \param Angle: rotation angle
		/// \param Center: center of rotation
		/// 
		/// \returns Reference to *this
		/// 
		VELOX_API Mat4f& Rotate(sf::Angle angle, const Vector2f& center);

		/// Builds a transform from the arguments.
		/// 
		/// \param Position: translation of matrix
		/// \param Origin: local origin of the object
		/// \param Scale: scaling factors
		/// \param Rot: rotation angle
		/// 
		/// \returns Reference to *this
		/// 
		VELOX_API Mat4f& Build(
			const Vector2f& position,
			const Vector2f& origin, 
			const Vector2f& scale, 
			sf::Angle rot);

		/// Builds a transform from the arguments.
		/// 
		/// \param Position: translation of matrix
		/// \param Scale: scaling factors
		/// \param Rot: rotation angle
		/// 
		/// \returns Reference to *this
		/// 
		VELOX_API Mat4f& Build(
			const Vector2f& position,
			const Vector2f& scale,
			sf::Angle rot);

		/// Builds a transform from the arguments.
		/// 
		/// \param Position: translation of matrix
		/// \param Rot: rotation angle
		/// 
		/// \returns Reference to *this
		/// 
		VELOX_API Mat4f& Build(
			const Vector2f& position,
			sf::Angle rot);

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

	constexpr const float* Mat4f::GetMatrix() const noexcept 
	{
		return m_matrix;
	}
	constexpr float Mat4f::Get(int x, int y) const
	{
		return m_matrix[x + y * 4];
	}

	constexpr Vector2f Mat4f::GetTranslation() const
	{
		return Vector2f(Get(0, 3), Get(1, 3));
	}

	constexpr Vector2f Mat4f::GetScale() const
	{
		return Vector2f(
			au::Sign(Get(0, 0)) * au::SqrtSqr(Get(0, 0), Get(1, 0)),
			au::Sign(Get(1, 1)) * au::SqrtSqr(Get(0, 1), Get(1, 1)));
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

	constexpr Mat2f Mat4f::GetRotationMatrix() const
	{
		return Mat2f(m_matrix[0],	m_matrix[4],
					 m_matrix[1],	m_matrix[5]);
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

	constexpr Mat4f& Mat4f::Build(const Vector2f& position)
	{
		const float tx = position.x;
		const float ty = position.y;

		m_transform = Mat4f( 1.f, 0.f, tx,
							 0.f, 1.f, ty,
							 0.f, 0.f, 1.f);

		return *this;
	}

	inline constexpr Mat4f Mat4f::Identity;
}