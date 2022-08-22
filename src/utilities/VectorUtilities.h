#pragma once

#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>

#include "Utilities.h"

namespace fge
{
	template<typename T> 
	struct v2 final
	{
	public:
		static constexpr const sf::Vector2<T> Direction(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return sf::Vector2<T>(to.x - from.x, to.y - from.y);
		}

		static constexpr float Length(const sf::Vector2<T>& vector)
		{
			return std::sqrtf(vector.x * vector.x + vector.y * vector.y);
		}
		static constexpr float LengthSq(const sf::Vector2<T>& vector)
		{
			return vector.x * vector.x + vector.y * vector.y;
		}
		static constexpr float LengthOpt(const sf::Vector2<T>& vector)
		{
			T dx = std::fabsf(vector.x);
			T dy = std::fabsf(vector.y);

			if (dy > dx)
				std::swap(dx, dy);

			return 1007.f / 1024.f * dx + 441.f / 1024.f * dy;
		}

		static constexpr float Length(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return Length(Direction(from, to));
		}
		static constexpr float LengthSq(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return LengthSq(Direction(from, to));
		}
		static constexpr float LengthOpt(const sf::Vector2<T>& from, const sf::Vector2<T>& to)
		{
			return LengthOpt(Direction(from, to));
		}

		static constexpr float Angle(const sf::Vector2<T>& vector)
		{
			return atan2f(vector.y, vector.x);
		}
		static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return acosf(Dot(lhs, rhs) / (Length(lhs) * Length(rhs)));
		}
		static constexpr float Angle(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, float lhs_length, float rhs_length)
		{
			return acosf(Dot(lhs, rhs) / (lhs_length * rhs_length));
		}

		static constexpr float Dot(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}
		static constexpr float Cross(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
		{
			return lhs.x * rhs.y - lhs.y * rhs.x;
		}

		static constexpr sf::Vector2<T> Normalize(sf::Vector2<T> vector, float radius = 1.0f)
		{
			float len = length(vector);

			if (len < FLT_EPSILON)
				return vector;

			vector.x *= (radius / len);
			vector.y *= (radius / len);

			return vector;
		}

		static constexpr sf::Vector2<T> Limit(const sf::Vector2<T>& vector, float max_length)
		{
			if (Length(vector) > max_length)
				return Normalize(vector, max_length);

			return vector;
		}

		static constexpr sf::Vector2<T> Clamp(const sf::Vector2<T>& vector, float max_length, float min_length)
		{
			float l = Length(vector);

			if (l > max_length)
				return Normalize(vector, max_length);
			if (l < min_length)
				return Normalize(vector, min_length);

			return vector;
		}


		static constexpr sf::Vector2<T> RotatePoint(const sf::Vector2<T>& point, const sf::Vector2<T>& center, float angle)
		{
			sf::Vector2<T> dir = Direction(center, point);

			float s = sinf(angle);
			float c = cosf(angle);

			return sf::Vector2<T>(
				(dir.x * c - dir.y * s) + center.x,
				(dir.x * s + dir.y * c) + center.y);
		}

		static constexpr sf::Vector2<T> Abs(sf::Vector2<T> vector)
		{
			return sf::Vector2<T>(std::fabsf(vector.x), std::fabsf(vector.y));
		}
		static constexpr sf::Vector2<T> Floor(sf::Vector2<T> vector)
		{
			return sf::Vector2<T>(std::floorf(vector.x), std::floorf(vector.y));
		}

		static constexpr const sf::Vector2<T> Lerp(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs, float a)
		{
			return sf::Vector3<T>(
				lerp(lhs.x, rhs.x, a),
				lerp(lhs.y, rhs.y, a));
		}
		static constexpr sf::Vector3<T> Lerp(const sf::Vector3<T>& lhs, const sf::Vector3<T>& rhs, float a)
		{
			return sf::Vector3<T>(
				lerp(lhs.x, rhs.x, a),
				lerp(lhs.y, rhs.y, a),
				lerp(lhs.z, rhs.z, a));
		}

	private:
		v2() = delete;
		~v2() = delete;
		v2(const v2&) = delete;
		v2(v2&&) = delete;
	};

	using v2i = typename v2<int>;
	using v2f = typename v2<float>;
	using v2d = typename v2<double>;

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

