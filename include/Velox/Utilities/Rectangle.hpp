#pragma once

#include <SFML/Graphics.hpp>

#include "Concepts.h"

namespace vlx
{
	template<Arithmetic T>
	struct Rect
	{
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

		[[nodiscard]] constexpr T Width() const noexcept { return (right - left); }
		[[nodiscard]] constexpr T Height() const noexcept { return (bot - top); }

		[[nodiscard]] constexpr sf::Vector2<T> GetSize() const noexcept
		{ 
			return sf::Vector2<T>(Width(), Height()); 
		}
		[[nodiscard]] constexpr T GetArea() const noexcept
		{ 
			return Width() * Height(); 
		}

		[[nodiscard]] constexpr sf::Vector2<T>& GetPosition() noexcept
		{
			return top_left;
		}
		[[nodiscard]] constexpr const sf::Vector2<T>& GetPosition() const noexcept
		{
			return const_cast<Rect<T>*>(this)->GetPosition();
		}

		[[nodiscard]] constexpr sf::Vector2<T> GetOrigin() const noexcept
		{
			return top_left + sf::Vector2<T>(Width() / 2.0f, Height() / 2.0f);
		}

		[[nodiscard]] constexpr bool Overlaps(const Rect<T>& other) const noexcept
		{
			return (left < other.right&& right >= other.left &&
				top < other.bot&& top >= other.top);
		}
		[[nodiscard]] constexpr bool Contains(const Rect<T>& other) const noexcept
		{
			return (other.left >= left && other.right < right&&
				other.top >= top && other.bot < bot);
		}
		[[nodiscard]] constexpr bool Contains(const sf::Vector2<T>& other) const noexcept
		{
			return !(other.x < left || other.x >= right ||
				other.y < top || other.y >= bot);
		}

		constexpr Rect<T> operator-() const noexcept
		{
			Rect<T> result(*this);
			result.top_left = -top_left;
			result.bot_right = -bot_right;
			return result;
		}

		constexpr Rect<T>& operator+=(const Rect<T>& rhs) noexcept
		{
			top_left += rhs.top_left;
			bot_right += rhs.bot_right;

			return *this;
		}
		constexpr Rect<T>& operator-=(const Rect<T>& rhs) noexcept
		{
			return *this += -rhs;
		}

		constexpr Rect<T> operator+(const Rect<T>& rhs) const noexcept
		{
			return (Rect<T>(*this) += rhs);
		}
		constexpr Rect<T> operator-(const Rect<T>& rhs) const noexcept
		{
			return *this + -rhs;
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