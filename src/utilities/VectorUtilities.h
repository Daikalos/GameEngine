#pragma once

#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>

#include "Utilities.h"

namespace fge
{
	namespace vu
	{
		template<typename T>
		static constexpr sf::Vector2<T> Direction(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return sf::Vector2<T>(to.x - from.x, to.y - from.y);
		}

		template<typename T>
		static constexpr float Distance(const sf::Vector2<T>& vector)
		{
			return std::sqrtf(vector.x * vector.x + vector.y * vector.y);
		}
		template<typename T>
		static constexpr float DistanceSq(const sf::Vector2<T>& vector)
		{
			return vector.x * vector.x + vector.y * vector.y;
		}
		template<typename T>
		static constexpr float DistanceOpt(const sf::Vector2<T>& vector)
		{
			T dx = std::fabsf(vector.x);
			T dy = std::fabsf(vector.y);

			if (dy > dx)
				std::swap(dx, dy);

			return 1007.f / 1024.f * dx + 441.f / 1024.f * dy;
		}

		template<typename T>
		static constexpr float Distance(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return Distance(Direction(from, to));
		}
		template<typename T>
		static constexpr float DistanceSq(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return DistanceSq(Direction(from, to));
		}
		template<typename T>
		static constexpr float DistanceOpt(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return DistanceOpt(Direction(from, to));
		}

		template<typename T>
		static constexpr float Angle(const sf::Vector2<T>& vector)
		{
			return atan2f(vector.y, vector.x);
		}
		template<typename T>
		static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, float lhs_length, float rhs_length)
		{
			return acosf(Dot(lhs, rhs) / (lhs_length * rhs_length));
		}
		template<typename T>
		static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return Angle(lhs, rhs, Distance(lhs), Distance(rhs));
		}

		template<typename T>
		static constexpr float Dot(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}
		template<typename T>
		static constexpr float Cross(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return lhs.x * rhs.y - lhs.y * rhs.x;
		}

		template<typename T>
		static constexpr sf::Vector2<T> Normalize(const sf::Vector2<T>& vector, float length, float radius)
		{
			if (length < FLT_EPSILON)
				return vector;

			vector.x *= (radius / length);
			vector.y *= (radius / length);

			return vector;
		}
		template<typename T>
		static constexpr sf::Vector2<T> Normalize(const sf::Vector2<T>& vector, float radius = 1.0f)
		{
			return Normalize(vector, Distance(vector), radius);
		}

		template<typename T>
		static constexpr sf::Vector2<T> Limit(const sf::Vector2<T>& vector, float length, float max_length)
		{
			if (length > max_length)
				return Normalize(vector, length, max_length);

			return vector;
		}
		template<typename T>
		static constexpr sf::Vector2<T> Limit(const sf::Vector2<T>& vector, float max_length)
		{
			return Limit(vector, Distance(vector), max_length);
		}

		template<typename T>
		static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, float length, float max_length, float min_length)
		{
			if (length > max_length)
				return Normalize(vector, length, max_length);
			if (length < min_length)
				return Normalize(vector, length, min_length);

			return vector;
		}
		template<typename T>
		static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, float max_length, float min_length)
		{
			return Clamp(vector, Distance(vector), max_length, min_length);
		}

		template<typename T>
		static constexpr sf::Vector2<T> RotatePoint(const sf::Vector2<T>& point, const sf::Vector2<T>& center, float angle)
		{
			sf::Vector2<T> dir = Direction(center, point);

			float s = sinf(angle);
			float c = cosf(angle);

			return sf::Vector2<T>(
				(dir.x * c - dir.y * s) + center.x,
				(dir.x * s + dir.y * c) + center.y);
		}

		template<typename T>
		static constexpr sf::Vector2<T> Abs(sf::Vector2<T> vector)
		{
			return sf::Vector2<T>(std::fabsf(vector.x), std::fabsf(vector.y));
		}
		template<typename T>
		static constexpr sf::Vector2<T> Floor(sf::Vector2<T> vector)
		{
			return sf::Vector2<T>(std::floorf(vector.x), std::floorf(vector.y));
		}

		template<typename T>
		static constexpr const sf::Vector2<T> Lerp(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, float a)
		{
			return sf::Vector3<T>(
				lerp(lhs.x, rhs.x, a),
				lerp(lhs.y, rhs.y, a));
		}
		template<typename T>
		static constexpr sf::Vector3<T> Lerp(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs, float a)
		{
			return sf::Vector3<T>(
				lerp(lhs.x, rhs.x, a),
				lerp(lhs.y, rhs.y, a),
				lerp(lhs.z, rhs.z, a));
		}
	}

	// operator overloads that SFML is missing

	template <typename T>
	static constexpr sf::Vector2<T> operator /=(sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		lhs.x /= rhs.x;
		lhs.y /= rhs.y;
		return lhs;
	}
	template <typename T>
	static constexpr sf::Vector2<T> operator /(float lhs, const sf::Vector2<T>& rhs)
	{
		sf::Vector2<T> result = { lhs, lhs };
		result.x /= rhs.x;
		result.y /= rhs.y;
		return result;
	}
	template <typename T>
	static constexpr sf::Vector2<T> operator /(sf::Vector2<T> lhs, const sf::Vector2<T>& rhs)
	{
		lhs.x /= rhs.x;
		lhs.y /= rhs.y;
		return lhs;
	}

	template <typename T>
	static constexpr sf::Vector2<T> operator *(sf::Vector2<T> lhs, const sf::Vector2<T>& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		return lhs;
	}

	template <typename T>
	static constexpr sf::Vector3<T> operator *(sf::Vector3<T> lhs, const sf::Vector3<T>& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		lhs.z *= rhs.z;
		return lhs;
	}
}

