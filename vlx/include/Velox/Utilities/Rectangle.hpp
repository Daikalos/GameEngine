#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/Config.hpp>

#include "Concepts.h"

namespace vlx
{
	template<Arithmetic T>
	struct Rect
	{
		constexpr Rect();
		constexpr Rect(T left, T top, T right, T bot);
		constexpr Rect(const sf::Vector2<T>& top_left, const sf::Vector2<T>& bot_right);

		template<typename U>
		constexpr explicit Rect(const Rect<U>& rect);

		NODISC constexpr bool operator==(const Rect& rhs) const;
		NODISC constexpr bool operator!=(const Rect& rhs) const;

		NODISC constexpr Rect operator-() const;

		constexpr Rect& operator+=(const Rect& rhs);
		constexpr Rect& operator-=(const Rect& rhs);

		NODISC constexpr Rect operator+(const Rect& rhs) const;
		NODISC constexpr Rect operator-(const Rect& rhs) const;

		NODISC constexpr T Width() const;
		NODISC constexpr T Height() const;

		NODISC constexpr sf::Vector2<T> Size() const;
		NODISC constexpr sf::Vector2<T> Center() const;

		NODISC constexpr T Area() const;

		NODISC constexpr Rect Union(const Rect& other) const;
		NODISC constexpr std::optional<Rect> Intersection(const Rect& other) const;

		NODISC constexpr bool Overlaps(const Rect& other) const;

		NODISC constexpr bool Contains(const sf::Vector2<T>& point) const;
		NODISC constexpr bool Contains(const Rect& other) const;

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

	using RectInt	= Rect<int>;
	using RectFloat = Rect<float>;

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect()
		: top_left({ T(), T() }), bot_right({ T(), T() }) {};

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(T left, T top, T right, T bot)
		: top_left({ left, top }), bot_right({ right, bot }) {};

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const sf::Vector2<T>& top_left, const sf::Vector2<T>& bot_right)
		: top_left(top_left), bot_right(bot_right) {};

	template<Arithmetic T>
	template<typename U>
	inline constexpr Rect<T>::Rect(const Rect<U>& rect) :
		top_left(sf::Vector2<T>(rect.top_left)),
		bot_right(sf::Vector2<T>(rect.bot_right)) { };

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator==(const Rect& rhs) const
	{
		return top_left == rhs.top_left && bot_right == rhs.bot_right;
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator!=(const Rect& rhs) const
	{
		return !(*this == rhs);
	}

	template<Arithmetic T>
	inline constexpr T Rect<T>::Width() const
	{ 
		return (right - left); 
	}
	template<Arithmetic T>
	inline constexpr T Rect<T>::Height() const
	{ 
		return (bot - top); 
	}

	template<Arithmetic T>
	inline constexpr sf::Vector2<T> Rect<T>::Size() const
	{ 
		return sf::Vector2<T>(Width(), Height()); 
	}

	template<Arithmetic T>
	inline constexpr sf::Vector2<T> Rect<T>::Center() const
	{
		return top_left + (Size() / T(2));
	}

	template<Arithmetic T>
	constexpr T Rect<T>::Area() const 
	{ 
		return Width() * Height();
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::Union(const Rect& other) const
	{
		// returns a rectangle that encompasses both rects

		const auto min = [](T a, T b) { return (a < b) ? a : b; };

		const T r1l = min(left, right);
		const T r1t = min(top, bot);
		const T r1r = (r1l == left) ? right : left;
		const T r1b = (r1t == top) ? bot : top;

		const T r2l = min(other.left, other.right);
		const T r2t = min(other.top, other.bot);
		const T r2r = (r2l == other.left) ? other.right : other.left;
		const T r2b = (r2t == other.top) ? other.bot : other.top;

		return Rect<T>(
			(r1l < r2l)	? r1l : r2l,
			(r1t < r2t)	? r1t : r2t,
			(r1r > r2r)	? r1r : r2r,
			(r1b > r2b)	? r1b : r2b);
	}
	template<Arithmetic T>
	inline constexpr std::optional<Rect<T>> Rect<T>::Intersection(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, right);
		const T r1t = min(top, bot);
		const T r1r = (r1l == left) ? right : left;
		const T r1b = (r1t == top) ? bot : top;

		const T r2l = min(other.left, other.right);
		const T r2t = min(other.top, other.bot);
		const T r2r = (r2l == other.left) ? other.right : other.left;
		const T r2b = (r2t == other.top) ? other.bot : other.top;

		const T ril = max(r1l, r2l);
		const T rit = max(r1t, r2t);
		const T rir = min(r1r, r2r);
		const T rib = min(r1b, r2b);

		if (ril < rir && rit < rib)
			return Rect(ril, rit, rir, rib);

		return std::nullopt;
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Overlaps(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, right);
		const T r1t = min(top, bot);
		const T r1r = (r1l == left) ? right : left;
		const T r1b = (r1t == top) ? bot : top;

		const T r2l = min(other.left, other.right);
		const T r2t = min(other.top, other.bot);
		const T r2r = (r2l == other.left) ? other.right : other.left;
		const T r2b = (r2t == other.top) ? other.bot : other.top;

		return !(r1l > r2r || r2l > r1r || r1t > r2b || r2t > r1b);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const sf::Vector2<T>& point) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };

		const T r1l = min(left, right);
		const T r1t = min(top, bot);
		const T r1r = (r1l == left) ? right : left;
		const T r1b = (r1t == top) ? bot : top;

		return (point.x >= r1l && point.x < r1r && point.y >= r1t && point.y < r1b);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, right);
		const T r1t = min(top, bot);
		const T r1r = (r1l == left) ? right : left;
		const T r1b = (r1t == top) ? bot : top;

		const T r2l = min(other.left, other.right);
		const T r2t = min(other.top, other.bot);
		const T r2r = (r2l == other.left) ? other.right : other.left;
		const T r2b = (r2t == other.top) ? other.bot : other.top;

		return (r2l >= r1l && r2r < r1r && r2t >= r1t && r2b < r1b);
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-() const
	{
		return Rect(-left, -top, -right, -bot);
	}

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator+=(const Rect& rhs)
	{
		top_left += rhs.top_left;
		bot_right += rhs.bot_right;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator-=(const Rect& rhs)
	{
		return (*this += -rhs);
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator+(const Rect& rhs) const
	{
		return Rect<T>(*this) += rhs;
	}
	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-(const Rect& rhs) const
	{
		return Rect<T>(*this) += -rhs;
	}
}