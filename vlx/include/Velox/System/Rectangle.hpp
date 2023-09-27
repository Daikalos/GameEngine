#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <Velox/Config.hpp>

#include "Concepts.h"
#include "Vector2.hpp"

namespace vlx
{
	template<Arithmetic T>
	class Rect
	{
	public:
		constexpr Rect();
		constexpr Rect(T left, T top, T width, T height);
		constexpr Rect(const Vector2<T>& position, const Vector2<T>& size);
		constexpr Rect(const sf::Rect<T>& rhs);

		template<Arithmetic U>
		constexpr explicit Rect(const Rect<U>& rhs);
		template<Arithmetic U>
		constexpr explicit Rect(const sf::Rect<U>& rhs);

	public:
		constexpr operator sf::Rect<T>&();
		constexpr operator const sf::Rect<T>&() const;

		NODISC constexpr Rect operator-() const;

		constexpr Rect& operator+=(const Rect& rhs);
		constexpr Rect& operator-=(const Rect& rhs);

		constexpr Rect& operator+=(const Vector2<T>& rhs);
		constexpr Rect& operator-=(const Vector2<T>& rhs);

		constexpr Rect& operator*=(const float rhs);

		NODISC constexpr Rect operator+(const Rect& rhs) const;
		NODISC constexpr Rect operator-(const Rect& rhs) const;

		NODISC constexpr Rect operator+(const Vector2<T>& rhs) const;
		NODISC constexpr Rect operator-(const Vector2<T>& rhs) const;

		NODISC constexpr Rect operator*(const float rhs) const;

		NODISC constexpr bool operator==(const Rect& rhs) const;
		NODISC constexpr bool operator!=(const Rect& rhs) const;

	public:
		NODISC constexpr T Right() const;
		NODISC constexpr T Bottom() const;

		NODISC constexpr Vector2<T> Position() const;
		NODISC constexpr Vector2<T> Size() const;
		NODISC constexpr Vector2<T> Center() const;

		NODISC constexpr T Area() const;

		NODISC constexpr Rect Union(const Rect& other) const;
		NODISC constexpr std::optional<Rect> Intersection(const Rect& other) const;

		NODISC constexpr bool Overlaps(const Rect& other) const;

		NODISC constexpr bool Contains(const Vector2<T>& point) const;
		NODISC constexpr bool Contains(const Rect& other) const;

		NODISC constexpr Rect<T> Inflate(const Vector2<T>& m) const;
		NODISC constexpr Rect<T> Inflate(T multiplier) const;

		union
		{
			sf::Rect<T> rect{};
			struct
			{
				T left, top, width, height;
			};
		};
	};

	using RectInt	= Rect<int>;
	using RectFloat = Rect<float>;

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect() = default;

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(T left, T top, T width, T height)
		: rect({ left, top }, { width, height }) { };

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const Vector2<T>& position, const Vector2<T>& size)
		: rect({ position.x, position.y }, { size.x, size.y }) { };

	template<Arithmetic T>
	inline constexpr Rect<T>::Rect(const sf::Rect<T>& rhs)
		: rect(rhs) { };

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Rect<T>::Rect(const Rect<U>& rhs)
		: rect({ rhs.left, rhs.top }, { rhs.width, rhs.height }) { };

	template<Arithmetic T>
	template<Arithmetic U>
	inline constexpr Rect<T>::Rect(const sf::Rect<U>& rhs)
		: rect(rhs) { };

	template<Arithmetic T>
	inline constexpr Rect<T>::operator sf::Rect<T>&()
	{
		return rect;
	}

	template<Arithmetic T>
	inline constexpr Rect<T>::operator const sf::Rect<T>&() const
	{
		return rect;
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator==(const Rect& rhs) const
	{
		return (rect == rhs.rect);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::operator!=(const Rect& rhs) const
	{
		return (rect != rhs.rect);
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
	inline constexpr Vector2<T> Rect<T>::Position() const
	{
		return rect.getPosition();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Rect<T>::Size() const
	{ 
		return rect.getSize();
	}

	template<Arithmetic T>
	inline constexpr Vector2<T> Rect<T>::Center() const
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
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left,	Right());
		const T r1t = min(top,	Bottom());
		const T r1r = max(left, Right());
		const T r1b = max(top,	Bottom());

		const T r2l = min(other.left,	other.Right());
		const T r2t = min(other.top,	other.Bottom());
		const T r2r = max(other.left,	other.Right());
		const T r2b = max(other.top,	other.Bottom());

		const T r3l = min(r1l, r2l);
		const T r3t = min(r1t, r2t);
		const T r3r = max(r1r, r2r);
		const T r3b = max(r1b, r2b);

		return Rect<T>(r3l, r3t, r3r - r3l, r3b - r3t);
	}
	template<Arithmetic T>
	inline constexpr std::optional<Rect<T>> Rect<T>::Intersection(const Rect& other) const
	{
		return rect.findIntersection(other);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Overlaps(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, Right());
		const T r1t = min(top,	Bottom());
		const T r1r = max(left, Right());
		const T r1b = max(top,	Bottom());

		const T r2l = min(other.left,	other.Right());
		const T r2t = min(other.top,	other.Bottom());
		const T r2r = max(other.left,	other.Right());
		const T r2b = max(other.top,	other.Bottom());

		return !(r1l > r2r || r2l > r1r || r1t > r2b || r2t > r1b);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const Vector2<T>& point) const
	{
		return rect.contains(point);
	}

	template<Arithmetic T>
	inline constexpr bool Rect<T>::Contains(const Rect& other) const
	{
		const auto min = [](T a, T b) { return (a < b) ? a : b; };
		const auto max = [](T a, T b) { return (a < b) ? b : a; };

		const T r1l = min(left, Right());
		const T r1t = min(top,	Bottom());
		const T r1r = max(left, Right());
		const T r1b = max(top,	Bottom());

		const T r2l = min(other.left,	other.Right());
		const T r2t = min(other.top,	other.Bottom());
		const T r2r = max(other.left,	other.Right());
		const T r2b = max(other.top,	other.Bottom());

		return (r2l >= r1l && r2r < r1r && r2t >= r1t && r2b < r1b);
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::Inflate(const Vector2<T>& m) const
	{
		Rect result(*this);

		float prev_width	= result.width;
		float prev_height	= result.height;

		result.width		*= m.x;
		result.height		*= m.y;

		result.left			-= (result.width - prev_width) / static_cast<T>(2);
		result.top			-= (result.height - prev_height) / static_cast<T>(2);

		return result;
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::Inflate(T multiplier) const
	{
		return Inflate({ multiplier, multiplier });
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
		return Rect(*this) += rhs;
	}
	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-(const Rect& rhs) const
	{
		return Rect(*this) += -rhs;
	}

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator+=(const Vector2<T>& rhs)
	{
		left += rhs.x;
		top += rhs.y;

		return *this;
	}
	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator-=(const Vector2<T>& rhs)
	{
		return (*this += -rhs);
	}

	template<Arithmetic T>
	inline constexpr Rect<T>& Rect<T>::operator*=(const float rhs)
	{
		width *= rhs;
		height *= rhs;

		return *this;
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator+(const Vector2<T>& rhs) const
	{
		return Rect<T>(*this) += rhs;
	}
	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator-(const Vector2<T>& rhs) const
	{
		return Rect<T>(*this) += -rhs;
	}

	template<Arithmetic T>
	inline constexpr Rect<T> Rect<T>::operator*(const float rhs) const
	{
		return Rect<T>(*this) *= rhs;
	}
}