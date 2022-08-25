#pragma once

#include <SFML/Graphics.hpp>

namespace vlx
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

		constexpr T Width() const { return (right - left); }
		constexpr T Height() const { return (bot - top); }

		constexpr sf::Vector2<T> GetSize() const 
		{ 
			return sf::Vector2<T>(Width(), Height()); 
		}
		constexpr T GetArea() const 
		{ 
			return Width() * Height(); 
		}

		constexpr sf::Vector2<T>& GetPosition()
		{
			return top_left;
		}
		constexpr const sf::Vector2<T>& GetPosition() const
		{
			return const_cast<Rect<T>*>(this)->GetPosition();
		}

		constexpr sf::Vector2<T> GetOrigin() const
		{
			return top_left + sf::Vector2<T>(Width() / 2.0f, Height() / 2.0f);
		}

		constexpr Rect<T> operator-() const
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

		constexpr bool Overlaps(const Rect<T>& other) const
		{
			return (left < other.right && right >= other.left &&
				top < other.bot && top >= other.top);
		}
		constexpr bool Contains(const Rect<T>& other) const
		{
			return (other.left >= left && other.right < right &&
				other.top >= top && other.bot < bot);
		}
		constexpr bool Contains(const sf::Vector2<T>& other) const
		{
			return !(other.x < left || other.x >= right ||
				other.y < top || other.y >= bot);
		}

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