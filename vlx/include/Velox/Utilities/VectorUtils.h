#pragma once

#define _USE_MATH_DEFINES

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <Velox/Config.hpp>

#include "ArithmeticUtils.h"
#include "Concepts.h"

namespace vlx::vu
{
	template<Arithmetic T>
	NODISC static constexpr auto Dot(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template<Arithmetic T>
	NODISC static constexpr auto Cross(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}
	template<Arithmetic T>
	NODISC static constexpr auto Cross(const sf::Vector2<T>& lhs, const float scalar)
	{
		return sf::Vector2<T>(scalar * lhs.y, -scalar * lhs.x);
	}
	template<Arithmetic T>
	NODISC static constexpr auto Cross(const float scalar, const sf::Vector2<T>& lhs)
	{
		return sf::Vector2<T>(-scalar * lhs.y, scalar * lhs.x);
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Direction(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
	{
		return sf::Vector2<T>(to.x - from.x, to.y - from.y);
	}

	template<Arithmetic T>
	NODISC static constexpr float DistanceOpt(const sf::Vector2<T>& vector)
	{
		T dx = std::abs(vector.x);
		T dy = std::abs(vector.y);

		if (dy > dx)
			std::swap(dx, dy);

		return 1007.f / 1024.f * dx + 441.f / 1024.f * dy;
	}

	template<Arithmetic T>
	NODISC static constexpr float Distance(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
	{
		return Direction(from, to).length();
	}
	template<Arithmetic T>
	NODISC static constexpr float DistanceSq(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
	{
		return Direction(from, to).lengthSq();
	}
	template<Arithmetic T>
	NODISC static constexpr float DistanceOpt(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
	{
		return DistanceOpt(Direction(from, to));
	}

	template<Arithmetic T>
	NODISC static constexpr float Angle(const sf::Vector2<T>& vector)
	{
		return atan2f(vector.y, vector.x);
	}
	template<Arithmetic T>
	NODISC static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, const float lhs_length, const float rhs_length)
	{
		return acosf(Dot(lhs, rhs) / (lhs_length * rhs_length));
	}
	template<Arithmetic T>
	NODISC static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return Angle(lhs, rhs, lhs.length(), rhs.length());
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Normalize(sf::Vector2<T> vector, const float length, const float radius)
	{
		if (length < FLT_EPSILON)
			return vector;

		vector.x *= (radius / length);
		vector.y *= (radius / length);

		return vector;
	}
	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Normalize(const sf::Vector2<T>& vector, const float radius = 1.0f)
	{
		return Normalize(vector, vector.length(), radius);
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Limit(const sf::Vector2<T>& vector, const float length, const float max_length)
	{
		if (length > max_length)
			return Normalize(vector, length, max_length);

		return vector;
	}
	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Limit(const sf::Vector2<T>& vector, const float max_length)
	{
		return Limit(vector, vector.length(), max_length);
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, const float length, const float max_length, const float min_length)
	{
		if (length > max_length)
			return Normalize(vector, length, max_length);
		if (length < min_length)
			return Normalize(vector, length, min_length);

		return vector;
	}
	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, const float max_length, const float min_length)
	{
		return Clamp(vector, vector.length(), max_length, min_length);
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, const sf::Vector2<T>& min, const sf::Vector2<T>& max)
	{
		return sf::Vector2<T>(
			std::clamp(vector.x, min.x, max.x),
			std::clamp(vector.y, min.y, max.y));
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> RotatePoint(const sf::Vector2<T>& point, const sf::Vector2<T>& center, const float angle)
	{
		const sf::Vector2<T> dir = Direction(center, point);

		const float s = std::sin(angle);
		const float c = std::cos(angle);

		return sf::Vector2<T>(
			(dir.x * c - dir.y * s) + center.x,
			(dir.x * s + dir.y * c) + center.y);
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Abs(sf::Vector2<T> vector)
	{
		return sf::Vector2<T>(std::abs(vector.x), std::abs(vector.y));
	}
	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Floor(sf::Vector2<T> vector)
	{
		return sf::Vector2<T>(std::floor(vector.x), std::floor(vector.y));
	}

	template<Arithmetic T>
	NODISC static constexpr sf::Vector2<T> Lerp(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, const float a)
	{
		return sf::Vector3<T>(
			Lerp(lhs.x, rhs.x, a),
			Lerp(lhs.y, rhs.y, a));
	}
	template<Arithmetic T>
	NODISC static constexpr sf::Vector3<T> Lerp(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs, const float a)
	{
		return sf::Vector3<T>(
			Lerp(lhs.x, rhs.x, a),
			Lerp(lhs.y, rhs.y, a),
			Lerp(lhs.z, rhs.z, a));
	}

	static const sf::Vector2f Zero = {};
}

namespace vlx
{
	// operator overloads that SFML is missing

	template <Arithmetic T>
	static constexpr sf::Vector2<T>& operator /=(sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		lhs.x /= rhs.x;
		lhs.y /= rhs.y;
		return lhs;
	}
	template <Arithmetic T>
	static constexpr sf::Vector2<T> operator /(const float lhs, const sf::Vector2<T>& rhs)
	{
		return { lhs / rhs.x, lhs / rhs.y };
	}

	template <Arithmetic T>
	static constexpr sf::Vector2<T> operator /(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return lhs.cwiseDiv(rhs);
	}
	template <Arithmetic T>
	static constexpr sf::Vector2<T> operator *(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return lhs.cwiseMul(rhs);
	}

	template <Arithmetic T>
	static constexpr sf::Vector3<T> operator /(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs)
	{
		return lhs.cwiseDiv(rhs);
	}
	template <Arithmetic T>
	static constexpr sf::Vector3<T> operator *(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs)
	{
		return lhs.cwiseMul(rhs);
	}
}

