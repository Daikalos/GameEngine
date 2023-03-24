#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/Config.hpp>

#include "Concepts.h"

namespace vlx
{
	/// Extended Vector2 based on sf::Vector2
	///
	template<Arithmetic T>
	class Vector2
	{
	public:
		constexpr Vector2();
		constexpr Vector2(T x, T y);
		constexpr Vector2(const sf::Vector2<T>& rhs);

		template <Arithmetic U>
		constexpr explicit Vector2(const Vector2<U>& rhs);
		template <Arithmetic U>
		constexpr explicit Vector2(const sf::Vector2<U>& rhs);

	public:
		constexpr operator sf::Vector2<T>&();
		constexpr operator const sf::Vector2<T>&() const;

		NODISC constexpr Vector2 operator-() const;

		constexpr Vector2& operator+=(const Vector2& rhs);
		constexpr Vector2& operator-=(const Vector2& rhs);

		constexpr Vector2& operator*=(const T rhs);
		constexpr Vector2& operator/=(const T rhs);

		constexpr Vector2& operator*=(const Vector2& rhs);
		constexpr Vector2& operator/=(const Vector2& rhs);

		NODISC constexpr Vector2 operator+(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator-(const Vector2& rhs) const;

		NODISC constexpr Vector2 operator*(const T rhs) const;
		NODISC constexpr Vector2 operator/(const T rhs) const;

		NODISC constexpr Vector2 operator*(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator/(const Vector2& rhs) const;

		NODISC constexpr bool operator==(const Vector2& rhs) const;
		NODISC constexpr bool operator!=(const Vector2& rhs) const;

	public:
		NODISC static constexpr auto Cross(const Vector2& lhs, const T scalar);
		NODISC static constexpr auto Cross(const T scalar, const Vector2& lhs);

		/// \returns Directional vector pointing between from and to.
		/// 
		NODISC static constexpr Vector2 Direction(const Vector2& from, const Vector2& to);

		/// Linear interpolation.
		/// 
		/// \param Alpha: Determines level of interpolation; range of 0.0f to 1.0f, where 0.0f yield lhs, and 1.0f yield rhs.
		/// 
		/// \returns Interpolated vector that ranges between lhs and rhs given by alpha value.
		///
		NODISC static constexpr Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, const float alpha);

		/// Rotates point around center.
		/// 
		/// \param Point: Point to rotate.
		/// \param Center: Point to rotate around.
		/// \param Angle: Number of degrees to rotate point.
		/// 
		/// \returns Rotated vector.
		///
		NODISC static constexpr Vector2 RotatePoint(const Vector2& point, const Vector2& center, const sf::Angle angle);

	public:
		///	Length of the vector; only works if the vector is of floating point type.
		/// 
		NODISC constexpr T Length() const requires FloatingPoint<T>;

		/// Square length of the vector; only works if the vector is of floating point type.
		/// 
		NODISC constexpr T LengthSq() const requires FloatingPoint<T>;

		/// Computes a normalized vector; only works if the vector is of floating point type.
		/// 
		/// \param Radius: Length of the normalized vector.
		/// 
		/// \returns Normalized vector.
		/// 
		NODISC constexpr Vector2<T> Normalize(const T radius = 1.0f) const requires FloatingPoint<T>;

		/// Computes a normalized vector; only works if the vector is of floating point type.
		/// 
		/// \param Length: Optimization param for when length has already been calculated.
		/// \param Radius: Length of the normalized vector.
		/// 
		/// \returns Normalized vector.
		/// 
		NODISC constexpr Vector2<T> Normalize(const T length, const T radius) const requires FloatingPoint<T>;

		/// Signed angle starting from +X or (1,0) vector; only works if the vector is of floating point type.
		/// 
		/// \returns Angle in the range of (-180, 180).
		/// 
		NODISC constexpr sf::Angle Angle() const requires FloatingPoint<T>;

		/// Signed angle going from this to rhs; only works if the vector is of floating point type.
		/// 
		/// \returns Smallest angle which rotates this to rhs, which can be either positive or negative.
		/// Result lies in the range (-180, 180)
		/// 
		NODISC constexpr sf::Angle AngleTo(const Vector2& rhs) const requires FloatingPoint<T>;

		/// Dot product of two vectors.
		/// 
		NODISC constexpr T Dot(const Vector2& rhs) const;

		/// Cross product of two vectors.
		/// 
		/// \returns The Z axis value.
		/// 
		NODISC constexpr T Cross(const Vector2& rhs) const;

		/// Rotate vector clockwise by angle; only works if the vector is of floating point type.
		/// 
		/// \returns Vector with different direction.
		/// 
		NODISC constexpr Vector2 RotatedBy(sf::Angle phi) const requires FloatingPoint<T>;

		/// Projects this vector onto axis; only works if the vector is of floating point type.
		/// 
		/// \returns Projected vector.
		/// 
		NODISC constexpr Vector2 ProjectedOnto(const Vector2& axis) const requires FloatingPoint<T>;

		/// Essentially rotates vector clockwise +90 degrees; only works if the vector is of floating point type.
		/// 
		/// \returns Perpendicular vector
		/// 
		NODISC constexpr Vector2 Perpendicular() const requires FloatingPoint<T>;

		/// Limits the length of the vector to a certain length; only works if the vector is of floating point type.
		/// 
		/// \param MaxLength: Length to limit by.
		/// 
		/// \returns Clamped vector if above length, otherwise returns an exact copy.
		/// 
		NODISC constexpr Vector2 Limit(const T max_length) const requires FloatingPoint<T>;

		/// Limits the length of the vector to a certain length; only works if the vector is of floating point type.
		/// 
		/// \param Length: Optimization param for when length has already been calculated.
		/// \param MaxLength: Length to limit by.
		/// 
		/// \returns Clamped vector if above length, otherwise returns an exact copy.
		/// 
		NODISC constexpr Vector2 Limit(const T length, const T max_length) const requires FloatingPoint<T>;

		/// Clamps vector x and y coordinates by the corresponding coordinates in min and max.
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise.
		/// 
		NODISC constexpr Vector2 Clamp(const Vector2& min, const Vector2& max) const;

		/// Clamps vector length by max length and min length.
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise.
		/// 
		NODISC constexpr Vector2 Clamp(const T max_length, const T min_length) const;

		/// Clamps vector length by max length and min length.
		/// 
		/// \param Length: Optimization param for when length has already been calculated.
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise.
		/// 
		NODISC constexpr Vector2 Clamp(const T length, const T max_length, const T min_length) const;

		/// \returns Vector containing the absolute values of this coordinates.
		///
		NODISC constexpr Vector2 Abs() const;

		/// \returns Vector containing the floor values of this coordinates.
		///
		NODISC constexpr Vector2 Floor() const;

	public:
		union
		{
			sf::Vector2<T> v{};
			struct
			{
				T x, y;
			};
		};

		static const Vector2<T> UnitX;
		static const Vector2<T> UnitY;
		static const Vector2<T> Zero;
		static const Vector2<T> One;
	};

	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int>;
	using Vector2u = Vector2<unsigned int>;

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::UnitX(static_cast<T>(1), static_cast<T>(0));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::UnitY(static_cast<T>(0), static_cast<T>(1));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Zero(static_cast<T>(0), static_cast<T>(0));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::One(static_cast<T>(1), static_cast<T>(1));

	template<Arithmetic T>
	inline constexpr Vector2<T>::Vector2() = default;

	template<Arithmetic T>
	inline constexpr Vector2<T>::Vector2(T x, T y)					: v(x, y) { }
	template<Arithmetic T>
	inline constexpr Vector2<T>::Vector2(const sf::Vector2<T>& rhs) : v(rhs) { }

	template<Arithmetic T> template<Arithmetic U>
	inline constexpr Vector2<T>::Vector2(const Vector2<U>& rhs)		: v(rhs.x, rhs.y) { }
	template<Arithmetic T> template<Arithmetic U>
	inline constexpr Vector2<T>::Vector2(const sf::Vector2<U>& rhs) : v(rhs) { }

	template<Arithmetic T>
	inline constexpr Vector2<T>::operator sf::Vector2<T>&()
	{
		return v;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T>::operator const sf::Vector2<T>&() const
	{
		return v;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2<T>(-x, -y);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& rhs)
	{
		return (*this += -rhs);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator*=(const T rhs)
	{
		x *= rhs;
		y *= rhs;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator/=(const T rhs)
	{
		x /= rhs;
		y /= rhs;

		return *this;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;

		return *this;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator+(const Vector2& rhs) const
	{
		return Vector2(*this) += rhs;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator-(const Vector2& rhs) const
	{
		return Vector2(*this) -= rhs;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator*(const T rhs) const
	{
		return Vector2(*this) *= rhs;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator/(const T rhs) const
	{
		return Vector2(*this) /= rhs;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator*(const Vector2& rhs) const
	{
		return Vector2(*this) *= rhs;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator/(const Vector2& rhs) const
	{
		return Vector2(*this) /= rhs;
	}

	template<Arithmetic T>
	inline constexpr bool Vector2<T>::operator==(const Vector2& rhs) const 
	{
		return v == rhs.v;
	}
	template<Arithmetic T>
	inline constexpr bool Vector2<T>::operator!=(const Vector2& rhs) const
	{
		return v != rhs.v;
	}

	template<Arithmetic T>
	inline constexpr auto Vector2<T>::Cross(const Vector2<T>& lhs, const T scalar)
	{
		return Vector2<T>(scalar * lhs.y, -scalar * lhs.x);
	}
	template<Arithmetic T>
	inline constexpr auto Vector2<T>::Cross(const T scalar, const Vector2<T>& lhs)
	{
		return Vector2<T>(-scalar * lhs.y, scalar * lhs.x);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Direction(const Vector2<T>& from, const Vector2<T>& to)
	{
		return Vector2<T>(to.x - from.x, to.y - from.y);
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Lerp(const Vector2<T>& lhs, const Vector2<T>& rhs, const float alpha)
	{
		return Vector2<T>(
			Lerp(lhs.x, rhs.x, alpha),
			Lerp(lhs.y, rhs.y, alpha));
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::RotatePoint(const Vector2<T>& point, const Vector2<T>& center, const sf::Angle angle)
	{
		const Vector2<T> dir = Direction(center, point);

		const float rad = angle.asRadians();

		const float s = std::sin(rad);
		const float c = std::cos(rad);

		return Vector2<T>(
			(dir.x * c - dir.y * s) + center.x,
			(dir.x * s + dir.y * c) + center.y);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Length() const requires FloatingPoint<T>
	{
		return v.length();
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::LengthSq() const requires FloatingPoint<T>
	{
		return v.lengthSq();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Normalize(const T radius) const requires FloatingPoint<T>
	{
		return Normalize(v.length(), radius);
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Normalize(const T length, const T radius) const requires FloatingPoint<T>
	{
		if (length < FLT_EPSILON || std::abs(length - radius) < FLT_EPSILON)
			return *this;

		return (*this) * (radius / length);
	}

	template<Arithmetic T>
	inline constexpr sf::Angle Vector2<T>::Angle() const requires FloatingPoint<T>
	{
		return v.angle();
	}

	template<Arithmetic T>
	inline constexpr sf::Angle Vector2<T>::AngleTo(const Vector2& rhs) const requires FloatingPoint<T>
	{
		return v.angleTo(rhs);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Dot(const Vector2& rhs) const
	{
		return v.dot(rhs);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Cross(const Vector2& rhs) const
	{
		return v.cross(rhs);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::RotatedBy(sf::Angle phi) const requires FloatingPoint<T>
	{
		return v.rotatedBy(phi);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::ProjectedOnto(const Vector2<T>& axis) const requires FloatingPoint<T>
	{
		return v.projectedOnto(axis);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Perpendicular() const requires FloatingPoint<T>
	{
		return v.perpendicular();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Limit(const T max_length) const requires FloatingPoint<T>
	{
		return Limit(v.length(), max_length);
	}
	template<Arithmetic T> 
	inline constexpr Vector2<T> Vector2<T>::Limit(const T length, const T max_length) const requires FloatingPoint<T>
	{
		if (length > max_length)
			return Normalize(length, max_length);

		return *this;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Clamp(const Vector2<T>& min, const Vector2<T>& max) const
	{ 
		return Vector2<T>(
			std::clamp(x, min.x, max.x),
			std::clamp(y, min.y, max.y));
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Clamp(const T max_length, const T min_length) const
	{
		return Clamp(v.length(), max_length, min_length);
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Clamp(const T length, const T max_length, const T min_length) const
	{
		if (length > max_length)
			return Normalize(length, max_length);
		if (length < min_length)
			return Normalize(length, min_length);

		return *this;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Abs() const
	{
		return Vector2<T>(std::abs(x), std::abs(y));
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Floor() const
	{
		return Vector2<T>(std::floor(x), std::floor(y));
	}

	template <Arithmetic T>
	static constexpr Vector2<T> operator *(const T lhs, const Vector2<T>& rhs)
	{
		return { lhs * rhs.x, lhs * rhs.y };
	}

	template <Arithmetic T>
	static constexpr Vector2<T> operator /(const T lhs, const Vector2<T>& rhs)
	{
		return { lhs / rhs.x, lhs / rhs.y };
	}
}

