#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/Utilities/Concepts.h>
#include <Velox/Utilities/ArithmeticUtils.h>
#include <Velox/Config.hpp>

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

		constexpr Vector2& operator*=(const float rhs);
		constexpr Vector2& operator/=(const float rhs);

		constexpr Vector2& operator*=(const Vector2& rhs);
		constexpr Vector2& operator/=(const Vector2& rhs);

		NODISC constexpr Vector2 operator+(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator-(const Vector2& rhs) const;

		NODISC constexpr Vector2 operator*(const float rhs) const;
		NODISC constexpr Vector2 operator/(const float rhs) const;

		NODISC constexpr Vector2 operator*(const Vector2& rhs) const;
		NODISC constexpr Vector2 operator/(const Vector2& rhs) const;

		NODISC constexpr bool operator==(const Vector2& rhs) const;
		NODISC constexpr bool operator!=(const Vector2& rhs) const;

	public:
		NODISC static constexpr auto Cross(const Vector2& lhs, const float scalar);
		NODISC static constexpr auto Cross(const float scalar, const Vector2& lhs);

		NODISC static constexpr Vector2 Direction(const Vector2& from, const Vector2& to);
		NODISC static constexpr Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, const float alpha);
		NODISC static constexpr Vector2 RotatePoint(const Vector2& point, const Vector2& center, const float angle);

	public:
		NODISC constexpr T Length() const;
		NODISC constexpr T LengthSq() const;

		NODISC constexpr Vector2<T> Normalize(const float radius = 1.0f) const;
		NODISC constexpr Vector2<T> Normalize(const float length, const float radius) const;

		NODISC constexpr sf::Angle Angle() const;
		NODISC constexpr sf::Angle AngleTo(const Vector2& rhs) const;

		NODISC constexpr T Dot(const Vector2& rhs) const;
		NODISC constexpr T Cross(const Vector2 rhs) const;

		NODISC constexpr Vector2 ProjectedOnto(const Vector2& axis) const;

		NODISC constexpr Vector2 Perpendicular() const;

		NODISC constexpr Vector2 Limit(const float max_length) const;
		NODISC constexpr Vector2 Limit(const float length, const float max_length) const;

		NODISC constexpr Vector2 Clamp(const Vector2& min, const Vector2& max) const;
		NODISC constexpr Vector2 Clamp(const float max_length, const float min_length) const;
		NODISC constexpr Vector2 Clamp(const float length, const float max_length, const float min_length) const;

		NODISC constexpr Vector2 Abs() const;
		NODISC constexpr Vector2 Floor() const;

	public:
		union
		{
			sf::Vector2<T> vector;
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
	inline constexpr Vector2<T>::Vector2(T x, T y) 
		: x(x), y(y) { }

	template<Arithmetic T>
	inline constexpr Vector2<T>::Vector2(const sf::Vector2<T>& rhs)
		: vector(rhs) { }

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Vector2<T>::Vector2(const Vector2<U>& rhs)
		: x(static_cast<T>(rhs.x)), y(static_cast<T>(rhs.y)) { }

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Vector2<T>::Vector2(const sf::Vector2<U>& rhs)
		: vector(rhs) { }

	template<Arithmetic T>
	inline constexpr Vector2<T>::operator sf::Vector2<T>&()
	{
		return vector;
	}

	template<Arithmetic T>
	inline constexpr Vector2<T>::operator const sf::Vector2<T>&() const
	{
		return vector;
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
	inline constexpr Vector2<T>& Vector2<T>::operator*=(const float rhs)
	{
		x *= rhs;
		y *= rhs;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T>& Vector2<T>::operator/=(const float rhs)
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
	inline constexpr Vector2<T> Vector2<T>::operator*(const float rhs) const
	{
		return Vector2(*this) *= rhs;
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::operator/(const float rhs) const
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
		return vector == rhs.vector;
	}
	template<Arithmetic T>
	inline constexpr bool Vector2<T>::operator!=(const Vector2& rhs) const
	{
		return vector != rhs.vector;
	}

	template<Arithmetic T>
	inline constexpr auto Vector2<T>::Cross(const Vector2<T>& lhs, const float scalar)
	{
		return Vector2<T>(scalar * lhs.y, -scalar * lhs.x);
	}
	template<Arithmetic T>
	inline constexpr auto Vector2<T>::Cross(const float scalar, const Vector2<T>& lhs)
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
	inline constexpr Vector2<T> Vector2<T>::RotatePoint(const Vector2<T>& point, const Vector2<T>& center, const float angle)
	{
		const Vector2<T> dir = Direction(center, point);

		const float s = std::sin(angle);
		const float c = std::cos(angle);

		return Vector2<T>(
			(dir.x * c - dir.y * s) + center.x,
			(dir.x * s + dir.y * c) + center.y);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Length() const
	{
		return vector.length();
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::LengthSq() const
	{
		return vector.lengthSq();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Normalize(const float radius) const
	{
		return Normalize(vector.length(), radius);
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Normalize(const float length, const float radius) const
	{
		if (length < FLT_EPSILON || std::abs(length - radius) < FLT_EPSILON)
			return *this;

		const float inv_len = (radius / length);

		return Vector2<T>(x * inv_len, y * inv_len);
	}

	template<Arithmetic T>
	inline constexpr sf::Angle Vector2<T>::Angle() const
	{
		return vector.angle();
	}

	template<Arithmetic T>
	inline constexpr sf::Angle Vector2<T>::AngleTo(const Vector2& rhs) const
	{
		return vector.angleTo(rhs);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Dot(const Vector2& rhs) const
	{
		return vector.dot(rhs);
	}

	template<Arithmetic T>
	inline constexpr T Vector2<T>::Cross(const Vector2 rhs) const
	{
		return vector.cross(rhs);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::ProjectedOnto(const Vector2<T>& axis) const
	{
		return vector.projectedOnto(axis);
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Perpendicular() const
	{
		return vector.perpendicular();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Limit(const float max_length) const
	{
		return Limit(vector.length(), max_length);
	}
	template<Arithmetic T> 
	inline constexpr Vector2<T> Vector2<T>::Limit(const float length, const float max_length) const
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
	inline constexpr Vector2<T> Vector2<T>::Clamp(const float max_length, const float min_length) const
	{
		return Clamp(vector.length(), max_length, min_length);
	}
	template<Arithmetic T>
	inline constexpr Vector2<T> Vector2<T>::Clamp(const float length, const float max_length, const float min_length) const
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
	static constexpr Vector2<T> operator *(const float lhs, const Vector2<T>& rhs)
	{
		return { lhs * rhs.x, lhs * rhs.y };
	}

	template <Arithmetic T>
	static constexpr Vector2<T> operator /(const float lhs, const Vector2<T>& rhs)
	{
		return { lhs / rhs.x, lhs / rhs.y };
	}
}

