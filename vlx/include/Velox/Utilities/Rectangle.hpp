#pragma once

#include <SFML/Graphics.hpp>

#include "Concepts.h"

namespace vlx
{
	template<Arithmetic T>
	struct Rect
	{
		constexpr Rect() = default;
		constexpr Rect(const T left, const T top, const T right, const T bot);
		constexpr Rect(const sf::Vector2<T> top_left, const sf::Vector2<T> bot_right);

		template<Arithmetic U>
		constexpr explicit Rect(const Rect<U>& rect);
		template<Arithmetic U>
		constexpr explicit Rect(Rect<U>&& rect);

		[[nodiscard]] constexpr T Width() const noexcept;
		[[nodiscard]] constexpr T Height() const noexcept;

		[[nodiscard]] constexpr T GetArea() const noexcept;
		[[nodiscard]] constexpr sf::Vector2<T> GetSize() const noexcept;

		[[nodiscard]] constexpr sf::Vector2<T> GetOrigin() const noexcept;

		[[nodiscard]] constexpr sf::Vector2<T>& GetPosition() noexcept;
		[[nodiscard]] constexpr const sf::Vector2<T>& GetPosition() const noexcept;

		[[nodiscard]] constexpr bool Overlaps(const Rect<T>& other) const noexcept;
		[[nodiscard]] constexpr bool Contains(const Rect<T>& other) const noexcept;
		[[nodiscard]] constexpr bool Contains(const sf::Vector2<T>& other) const noexcept;

		[[nodiscard]] constexpr Rect<T> operator-() const noexcept;

		[[nodiscard]] constexpr Rect<T> operator+(const Rect<T>& rhs) const noexcept;
		[[nodiscard]] constexpr Rect<T> operator-(const Rect<T>& rhs) const noexcept;

		constexpr Rect<T>& operator+=(const Rect<T>& rhs) noexcept;
		constexpr Rect<T>& operator-=(const Rect<T>& rhs) noexcept;

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

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const T left, const T top, const T right, const T bot)
			: left(left), top(top), right(right), bot(bot) {};

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const sf::Vector2<T> top_left, const sf::Vector2<T> bot_right)
		: top_left(top_left), bot_right(bot_right) {};

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Rect<T>::Rect(const Rect<U>& rect) :
		top_left(sf::Vector2<T>(rect.top_left)),
		bot_right(sf::Vector2<T>(rect.bot_right)) { };

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Rect<T>::Rect(Rect<U>&& rect) :
		top_left(std::move(sf::Vector2<T>(rect.top_left))),
		bot_right(std::move(sf::Vector2<T>(rect.bot_right))) { };

	template<Arithmetic T>
	[[nodiscard]] inline constexpr T Rect<T>::Width() const noexcept { return (right - left); }
	template<Arithmetic T>
	[[nodiscard]] inline constexpr T Rect<T>::Height() const noexcept { return (bot - top); }

	template<Arithmetic T>
	[[nodiscard]] inline constexpr T Rect<T>::GetArea() const noexcept
	{
		return Width() * Height();
	}
	template<Arithmetic T>
	[[nodiscard]] inline constexpr sf::Vector2<T> Rect<T>::GetSize() const noexcept
	{
		return sf::Vector2<T>(Width(), Height());
	}

	template<Arithmetic T>
	[[nodiscard]] inline constexpr sf::Vector2<T> Rect<T>::GetOrigin() const noexcept
	{
		return top_left + sf::Vector2<T>(Width() / 2.0f, Height() / 2.0f);
	}

	template<Arithmetic T>
	[[nodiscard]] inline constexpr sf::Vector2<T>& Rect<T>::GetPosition() noexcept
	{
		return top_left;
	}
	template<Arithmetic T>
	[[nodiscard]] inline constexpr const sf::Vector2<T>& Rect<T>::GetPosition() const noexcept
	{
		return const_cast<Rect<T>*>(this)->GetPosition();
	}

	template<Arithmetic T>
	[[nodiscard]] inline constexpr bool Rect<T>::Overlaps(const Rect<T>& other) const noexcept
	{
		return (left < other.right&& right >= other.left &&
			top < other.bot&& top >= other.top);
	}
	template<Arithmetic T>
	[[nodiscard]] inline constexpr bool Rect<T>::Contains(const Rect<T>& other) const noexcept
	{
		return (other.left >= left && other.right < right&&
			other.top >= top && other.bot < bot);
	}
	template<Arithmetic T>
	[[nodiscard]] inline constexpr bool Rect<T>::Contains(const sf::Vector2<T>& other) const noexcept
	{
		return !(other.x < left || other.x >= right ||
			other.y < top || other.y >= bot);
	}

	template<Arithmetic T>
	[[nodiscard]] inline constexpr Rect<T> Rect<T>::operator-() const noexcept
	{
		Rect<T> result(*this);
		result.top_left = -top_left;
		result.bot_right = -bot_right;
		return result;
	}

	template<Arithmetic T>
	[[nodiscard]] inline constexpr Rect<T> Rect<T>::operator+(const Rect<T>& rhs) const noexcept
	{
		return (Rect<T>(*this) += rhs);
	}
	template<Arithmetic T>
	[[nodiscard]] inline constexpr Rect<T> Rect<T>::operator-(const Rect<T>& rhs) const noexcept
	{
		return *this + -rhs;
	}

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator+=(const Rect<T>& rhs) noexcept
	{
		top_left += rhs.top_left;
		bot_right += rhs.bot_right;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator-=(const Rect<T>& rhs) noexcept
	{
		return *this += -rhs;
	}

	using RectInt = Rect<int>;
	using RectFloat = Rect<float>;
}