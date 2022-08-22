#pragma once

#include <SFML/Graphics.hpp>

namespace fge
{
	template<typename T>
	struct Rect
	{
		static_assert(std::is_arithmetic_v<T>, "Only arithmetic type allowed");

		constexpr Rect() = default;
		constexpr Rect(T left, T top, T right, T bot)
			: top_left({ left, top }), bot_right({ right, bot }) {};
		constexpr Rect(sf::Vector2<T> top_left, sf::Vector2<T> bot_right)
			: top_left(top_left), bot_right(bot_right) {};

		template<typename U>
		constexpr explicit Rect(const Rect<U>& rect) :
			top_left(sf::Vector2<T>(rect.top_left)),
			bot_right(sf::Vector2<T>(rect.bot_right)) { };

		template<typename U>
		constexpr explicit Rect(Rect<U>&& rect) :
			top_left(std::move(sf::Vector2<T>(rect.top_left))),
			bot_right(std::move(sf::Vector2<T>(rect.bot_right))) { };

		constexpr Rect<T> operator-()
		{
			Rect<T> result(*this);
			result.top_left = -top_left;
			result.bot_right = -bot_right;
			return result;
		}

		constexpr Rect<T>& operator+=(const Rect<T>& rhs)
		{
			top_left += rhs.top_left;
			bot_right += rhs.bot_right;

			return *this;
		}
		constexpr Rect<T>& operator-=(const Rect<T>& rhs)
		{
			return *this += -rhs;
		}

		constexpr Rect<T> operator+(const Rect<T>& rhs) const
		{
			return (Rect<T>(*this) += rhs);
		}
		constexpr Rect<T> operator-(const Rect<T>& rhs) const
		{
			return *this + -rhs;
		}

		constexpr bool Intersects(const Rect<T>& other)
		{
			return !(left > other.right || right < other.left || 
				top > other.bot || bot < other.top);
		}
		constexpr bool Contains(const Rect<T>& other)
		{
			return (right >= other.left && left <= other.right &&
				top >= other.top && bot <= other.bot);
		}
		constexpr bool Contains(const sf::Vector2<T>& other)
		{
			return !(left > other.x || right < other.x ||
				top > other.y || bot < other.top);
		}

		constexpr sf::Vector2<T>& GetPosition()
		{
			return GetPosition();
		}
		constexpr const sf::Vector2<T>& GetPosition() const
		{
			return top_left;
		}

		constexpr T Width() const { return (right - left); }
		constexpr T Height() const { return (bot - top); }

		constexpr sf::Vector2<T> GetSize() const { return sf::Vector2<T>(Width(), Height()); }
		constexpr T GetArea() const { return Width() * Height(); }

		union
		{
			sf::Vector2<T> top_left;
			struct { T left, top; };
		};
		union
		{
			sf::Vector2<T> bot_right;
			struct { T right, bot; };
		};
	};

	using RectInt = Rect<int>;
	using RectFloat = Rect<float>;
}