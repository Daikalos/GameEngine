#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Velox/Config.hpp>

#include "Concepts.h"

namespace vlx
{
	template<Arithmetic T>
	struct Rect : public sf::Rect<T>
	{
		constexpr Rect();
		constexpr Rect(T left, T top, T width, T height);
		constexpr Rect(const sf::Vector2<T>& position, const sf::Vector2<T>& size);

		template<typename U>
		constexpr explicit Rect(const Rect<U>& rect);

		NODISC constexpr bool operator==(const Rect& rhs) const;
		NODISC constexpr bool operator!=(const Rect& rhs) const;

		NODISC constexpr Rect operator-() const;

		constexpr Rect& operator+=(const Rect& rhs);
		constexpr Rect& operator-=(const Rect& rhs);

		NODISC constexpr Rect operator+(const Rect& rhs) const;
		NODISC constexpr Rect operator-(const Rect& rhs) const;

		constexpr Rect& operator+=(const sf::Vector2<T>& rhs);
		constexpr Rect& operator-=(const sf::Vector2<T>& rhs);

		NODISC constexpr Rect operator+(const sf::Vector2<T>& rhs) const;
		NODISC constexpr Rect operator-(const sf::Vector2<T>& rhs) const;

		NODISC constexpr T Right() const;
		NODISC constexpr T Bottom() const;

		NODISC constexpr sf::Vector2<T> Position() const;
		NODISC constexpr sf::Vector2<T> Size() const;
		NODISC constexpr sf::Vector2<T> Center() const;

		NODISC constexpr T Area() const;

		NODISC constexpr Rect Union(const Rect& other) const;
		NODISC constexpr std::optional<Rect> Intersection(const Rect& other) const;

		NODISC constexpr bool Overlaps(const Rect& other) const;

		NODISC constexpr bool Contains(const sf::Vector2<T>& point) const;
		NODISC constexpr bool Contains(const Rect& other) const;

		using sf::Rect<T>::left;
		using sf::Rect<T>::top;
		using sf::Rect<T>::width;
		using sf::Rect<T>::height;
	};

	using RectInt	= Rect<int>;
	using RectFloat = Rect<float>;

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect() : sf::Rect<T>() {};

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(T left, T top, T width, T height)
		: sf::Rect<T>({ left, top }, { width, height }) {};

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const sf::Vector2<T>& position, const sf::Vector2<T>& size)
		: sf::Rect<T>(position, size) {};

	template<Arithmetic T>
	template<typename U>
	inline constexpr Rect<T>::Rect(const Rect<U>& rect) 
		: sf::Rect<T>(rect) {};

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator==(const Rect& rhs) const
	{
		return *this == rhs;
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator!=(const Rect& rhs) const
	{
		return *this != rhs;
	}

	template<Arithmetic T>
	inline constexpr T Rect<T>::Right() const
	{
		return static_cast<T>(left + width);
	}
	template<Arithmetic T>
	inline constexpr T Rect<T>::Bottom() const
	{
		return static_cast<T>(top + height);
	}

	template<Arithmetic T>
	inline constexpr sf::Vector2<T> Rect<T>::Position() const
	{
		return this->getPosition();
	}

	template<Arithmetic T>
	inline constexpr sf::Vector2<T> Rect<T>::Size() const
	{ 
		return this->getSize(); 
	}

	template<Arithmetic T>
	inline constexpr sf::Vector2<T> Rect<T>::Center() const
	{
		return Position() + (Size() / T(2));
	}

	template<Arithmetic T>
	constexpr T Rect<T>::Area() const 
	{ 
		return width * height;
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::Union(const Rect& other) const
	{
		// returns a rectangle that encompasses both rects

		const auto min = [](T a, T b) { return (a < b) ? a : b; };

		const T r1l = min(left, Right());
		const T r1t = min(top, Bottom());
		const T r1r = (r1l == left) ? Right() : left;
		const T r1b = (r1t == top) ? Bottom() : top;

		const T r2l = min(other.left, other.Right());
		const T r2t = min(other.top, other.Bottom());
		const T r2r = (r2l == other.left) ? other.Right() : other.left;
		const T r2b = (r2t == other.top) ? other.Bottom() : other.top;

		return Rect<T>(
			(r1l < r2l)	? r1l : r2l,
			(r1t < r2t)	? r1t : r2t,
			(r1r > r2r)	? r1r : r2r,
			(r1b > r2b)	? r1b : r2b);
	}
	template<Arithmetic T>
	inline constexpr std::optional<Rect<T>> Rect<T>::Intersection(const Rect& other) const
	{
		return findIntersection(other);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Overlaps(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, Right());
		const T r1t = min(top, Bottom());
		const T r1r = (r1l == left) ? Right() : left;
		const T r1b = (r1t == top) ? Bottom() : top;

		const T r2l = min(other.left, other.Right());
		const T r2t = min(other.top, other.Bottom());
		const T r2r = (r2l == other.left) ? other.Right() : other.left;
		const T r2b = (r2t == other.top) ? other.Bottom() : other.top;

		return !(r1l > r2r || r2l > r1r || r1t > r2b || r2t > r1b);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const sf::Vector2<T>& point) const
	{
		return contains(point);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, Right());
		const T r1t = min(top, Bottom());
		const T r1r = (r1l == left) ? Right() : left;
		const T r1b = (r1t == top) ? Bottom() : top;

		const T r2l = min(other.left, other.Right());
		const T r2t = min(other.top, other.Bottom());
		const T r2r = (r2l == other.left) ? other.Right() : other.left;
		const T r2b = (r2t == other.top) ? other.Bottom() : other.top;

		return (r2l >= r1l && r2r < r1r && r2t >= r1t && r2b < r1b);
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-() const
	{
		return Rect(-left, -top, -width, -height);
	}

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator+=(const Rect& rhs)
	{
		left += rhs.left;
		top += rhs.top;
		width += rhs.width;
		height += rhs.height;

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

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator+=(const sf::Vector2<T>& rhs)
	{
		left += rhs.x;
		top += rhs.y;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator-=(const sf::Vector2<T>& rhs)
	{
		return (*this += -rhs);
	}
	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator+(const sf::Vector2<T>& rhs) const
	{
		return Rect<T>(*this) += rhs;
	}
	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-(const sf::Vector2<T>& rhs) const
	{
		return Rect<T>(*this) += -rhs;
	}
}