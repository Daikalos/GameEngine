#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utility/ArithmeticUtils.h>

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
		constexpr Vector2(const sf::Vector2<T>& rhs); // allow implicit cast

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

		constexpr Vector2& operator*=(T rhs);
		constexpr Vector2& operator/=(T rhs);

		constexpr Vector2& operator*=(const Vector2& rhs);
		constexpr Vector2& operator/=(const Vector2& rhs);

		NODISC constexpr Vector2 operator+(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator-(const Vector2& rhs) const;

		NODISC constexpr Vector2 operator*(T rhs) const;
		NODISC constexpr Vector2 operator/(T rhs) const;

		NODISC constexpr Vector2 operator*(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator/(const Vector2& rhs) const;

		NODISC constexpr bool operator==(const Vector2& rhs) const;
		NODISC constexpr bool operator!=(const Vector2& rhs) const;

	public:
		NODISC constexpr static auto Cross(const Vector2& lhs, T scalar);
		NODISC constexpr static auto Cross(T scalar, const Vector2& lhs);

		/// \returns Directional vector pointing between from and to.
		/// 
		NODISC constexpr static Vector2 Direction(const Vector2& from, const Vector2& to);

		/// Linear interpolation.
		/// 
		/// \param Alpha: Determines state of interpolation, where 0.0f yield lhs, and 1.0f yield rhs
		/// 
		/// \returns Interpolated vector that ranges between lhs and rhs given by alpha value
		///
		NODISC constexpr static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float alpha);

		/// Rotates point around center.
		/// 
		/// \param Point: Point to rotate
		/// \param Center: Point to rotate around
		/// \param Angle: Number of degrees to rotate point
		/// 
		/// \returns Rotated vector
		///
		NODISC VELOX_API static Vector2 RotatePoint(const Vector2& point, const Vector2& center, sf::Angle angle) requires FloatingPoint<T>;

	public:
		///	Length of the vector; only works if the vector is of floating point type.
		/// 
		NODISC constexpr T Length() const requires FloatingPoint<T>;

		/// Square length of the vector; only works if the vector is of floating point type.
		/// 
		NODISC constexpr T LengthSq() const requires FloatingPoint<T>;

		/// Computes a normalized vector; only works if the vector is of floating point type.
		/// 
		/// \param Radius: Length of the normalized vector
		/// 
		/// \returns Normalized vector
		/// 
		NODISC constexpr Vector2<T> Normalize(T radius = 1.0f) const requires FloatingPoint<T>;

		/// Computes a normalized vector; only works if the vector is of floating point type.
		/// 
		/// \param Length: Optimization param for when length has already been calculated
		/// \param Radius: Length of the normalized vector
		/// 
		/// \returns Normalized vector
		/// 
		NODISC constexpr Vector2<T> Normalize(T length, T radius) const requires FloatingPoint<T>;

		/// Signed angle starting from +X or (1,0) vector; only works if the vector is of floating point type.
		/// 
		/// \returns Angle in the range of (-180, 180)
		/// 
		NODISC VELOX_API sf::Angle Angle() const requires FloatingPoint<T>;

		/// Signed angle going from this to rhs; only works if the vector is of floating point type.
		/// 
		/// \returns Smallest angle which rotates this to rhs, which can be either positive or negative, lies in the range (-180, 180)
		/// 
		NODISC VELOX_API sf::Angle AngleTo(const Vector2& rhs) const requires FloatingPoint<T>;

		/// Dot product of two vectors.
		/// 
		NODISC constexpr T Dot(const Vector2& rhs) const;

		/// Cross product of two vectors.
		/// 
		/// \returns The Z axis value
		/// 
		NODISC constexpr T Cross(const Vector2& rhs) const;

		/// Rotate vector clockwise by angle; only works if the vector is of floating point type.
		/// 
		/// \returns Vector with different direction
		/// 
		NODISC VELOX_API Vector2 RotatedBy(sf::Angle phi) const requires FloatingPoint<T>;

		/// Projects this vector onto axis; only works if the vector is of floating point type.
		/// 
		/// \returns Projected vector
		/// 
		NODISC constexpr Vector2 ProjectedOnto(const Vector2& axis) const requires FloatingPoint<T>;

		/// Essentially rotates vector clockwise +90 degrees.
		/// 
		/// \returns Perpendicular vector
		/// 
		NODISC constexpr Vector2 Perpendicular() const;

		/// Essentially rotates vector counter clockwise +90 degrees.
		/// 
		/// \returns Perpendicular vector
		/// 
		NODISC constexpr Vector2 Orthogonal() const;

		/// Limits the length of the vector to a certain length; only works if the vector is of floating point type.
		/// 
		/// \param MaxLength: Length to limit by
		/// 
		/// \returns Clamped vector if above length, otherwise returns an exact copy
		/// 
		NODISC constexpr Vector2 Limit(T max_length) const requires FloatingPoint<T>;

		/// Limits the length of the vector to a certain length; only works if the vector is of floating point type.
		/// 
		/// \param Length: Optimization param for when length has already been calculated
		/// \param MaxLength: Length to limit by
		/// 
		/// \returns Clamped vector if above length, otherwise returns an exact copy
		/// 
		NODISC constexpr Vector2 Limit(T length, T max_length) const requires FloatingPoint<T>;

		/// Clamps vector x and y coordinates by the corresponding coordinates in min and max.
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise
		/// 
		NODISC constexpr Vector2 Clamp(const Vector2& min, const Vector2& max) const;

		/// Clamps vector length by max length and min length.
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise
		/// 
		NODISC constexpr Vector2 Clamp(T max_length, T min_length) const;

		/// Clamps vector length by max length and min length.
		/// 
		/// \param Length: Optimization param for when length may already been calculated
		/// 
		/// \returns Clamped vector if outside min or max, an exact copy otherwise
		/// 
		NODISC constexpr Vector2 Clamp(T length, T max_length, T min_length) const;

		/// \returns Vector containing the absolute values of this coordinates
		///
		NODISC VELOX_API Vector2 Abs() const requires FloatingPoint<T>;

		/// \returns Vector containing the floor values of this coordinates
		///
		NODISC VELOX_API Vector2 Floor() const requires FloatingPoint<T>;

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

		static const Vector2<T> Up;
		static const Vector2<T> Down;
		static const Vector2<T> Left;
		static const Vector2<T> Right;
	};

	// using declarations

	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
	using Vector2i = Vector2<int>;
	using Vector2u = Vector2<unsigned int>;

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::UnitX(	static_cast<T>(1), static_cast<T>(0));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::UnitY(	static_cast<T>(0), static_cast<T>(1));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Zero(	static_cast<T>(0), static_cast<T>(0));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::One(	static_cast<T>(1), static_cast<T>(1));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Up(		static_cast<T>(0), static_cast<T>(1));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Down(	static_cast<T>(0), static_cast<T>(-1));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Left(	static_cast<T>(-1), static_cast<T>(0));

	template<Arithmetic T>
	inline const Vector2<T> Vector2<T>::Right(	static_cast<T>(0), static_cast<T>(1));

	template<Arithmetic T>
	constexpr Vector2<T>::Vector2() = default;

	template<Arithmetic T>
	constexpr Vector2<T>::Vector2(T x, T y)						: v(x, y) { }
	template<Arithmetic T>
	constexpr Vector2<T>::Vector2(const sf::Vector2<T>& rhs)	: v(rhs) { }

	template<Arithmetic T> template<Arithmetic U>
	constexpr Vector2<T>::Vector2(const Vector2<U>& rhs)		: x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)) { }
	template<Arithmetic T> template<Arithmetic U>
	constexpr Vector2<T>::Vector2(const sf::Vector2<U>& rhs)	: v(rhs) { }

	template<Arithmetic T>
	constexpr Vector2<T>::operator sf::Vector2<T>&()
	{
		return v;
	}

	template<Arithmetic T>
	constexpr Vector2<T>::operator const sf::Vector2<T>&() const
	{
		return v;
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2<T>(-x, -y);
	}

	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}
	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& rhs)
	{
		return (*this += -rhs);
	}

	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator*=(T rhs)
	{
		x *= rhs;
		y *= rhs;

		return *this;
	}
	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator/=(T rhs)
	{
		x /= rhs;
		y /= rhs;

		return *this;
	}

	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;

		return *this;
	}
	template<Arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;

		return *this;
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator+(const Vector2& rhs) const
	{
		return Vector2(*this) += rhs;
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator-(const Vector2& rhs) const
	{
		return Vector2(*this) -= rhs;
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator*(T rhs) const
	{
		return Vector2(*this) *= rhs;
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator/(T rhs) const
	{
		return Vector2(*this) /= rhs;
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator*(const Vector2& rhs) const
	{
		return Vector2(*this) *= rhs;
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator/(const Vector2& rhs) const
	{
		return Vector2(*this) /= rhs;
	}

	template<Arithmetic T>
	constexpr bool Vector2<T>::operator==(const Vector2& rhs) const 
	{
		return v == rhs.v;
	}
	template<Arithmetic T>
	constexpr bool Vector2<T>::operator!=(const Vector2& rhs) const
	{
		return v != rhs.v;
	}

	template<Arithmetic T>
	constexpr auto Vector2<T>::Cross(const Vector2<T>& lhs, T scalar)
	{
		return Vector2<T>(scalar * lhs.y, -scalar * lhs.x);
	}
	template<Arithmetic T>
	constexpr auto Vector2<T>::Cross(T scalar, const Vector2<T>& lhs)
	{
		return Vector2<T>(-scalar * lhs.y, scalar * lhs.x);
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Direction(const Vector2<T>& from, const Vector2<T>& to)
	{
		return Vector2<T>(to.x - from.x, to.y - from.y);
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Lerp(const Vector2<T>& lhs, const Vector2<T>& rhs, float alpha)
	{
		return Vector2<T>(
			au::Lerp(lhs.x, rhs.x, alpha),
			au::Lerp(lhs.y, rhs.y, alpha));
	}

	template<Arithmetic T>
	constexpr T Vector2<T>::Length() const requires FloatingPoint<T>
	{
		return v.length();
	}

	template<Arithmetic T>
	constexpr T Vector2<T>::LengthSq() const requires FloatingPoint<T>
	{
		return v.lengthSq();
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Normalize(T radius) const requires FloatingPoint<T>
	{
		return Normalize(v.length(), radius);
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Normalize(T length, T radius) const requires FloatingPoint<T>
	{
		if (length < FLT_EPSILON || std::abs(length - radius) < FLT_EPSILON)
			return *this;

		return (*this) * (radius / length);
	}

	template<Arithmetic T>
	constexpr T Vector2<T>::Dot(const Vector2& rhs) const
	{
		return v.dot(rhs);
	}

	template<Arithmetic T>
	constexpr T Vector2<T>::Cross(const Vector2& rhs) const
	{
		return v.cross(rhs);
	}


	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::ProjectedOnto(const Vector2<T>& axis) const requires FloatingPoint<T>
	{
		return v.projectedOnto(axis);
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Perpendicular() const
	{
		return v.perpendicular();
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Orthogonal() const
	{
		return -v.perpendicular();
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Limit(T max_length) const requires FloatingPoint<T>
	{
		return Limit(v.length(), max_length);
	}
	template<Arithmetic T> 
	constexpr Vector2<T> Vector2<T>::Limit(T length, T max_length) const requires FloatingPoint<T>
	{
		if (length > max_length)
			return Normalize(length, max_length);

		return *this;
	}

	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Clamp(const Vector2<T>& min, const Vector2<T>& max) const
	{ 
		return Vector2<T>(
			std::clamp(x, min.x, max.x),
			std::clamp(y, min.y, max.y));
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Clamp(T max_length, T min_length) const
	{
		return Clamp(v.length(), max_length, min_length);
	}
	template<Arithmetic T>
	constexpr Vector2<T> Vector2<T>::Clamp(T length, T max_length, T min_length) const
	{
		if (length > max_length)
			return Normalize(length, max_length);
		if (length < min_length)
			return Normalize(length, min_length);

		return *this;
	}

	// GLOBAL OPERATORS

	template <Arithmetic T>
	constexpr Vector2<T> operator *(T lhs, const Vector2<T>& rhs)
	{
		return { lhs * rhs.x, lhs * rhs.y };
	}

	template <Arithmetic T>
	constexpr Vector2<T> operator -(const sf::Vector2<T> lhs, const Vector2<T>& rhs)
	{
		return Vector2<T>(lhs) - rhs;
	}

	template <Arithmetic T>
	constexpr Vector2<T> operator /(T lhs, const Vector2<T>& rhs)
	{
		return { lhs / rhs.x, lhs / rhs.y };
	}
}

