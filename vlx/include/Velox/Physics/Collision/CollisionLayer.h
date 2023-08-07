#pragma once

#include <stdint.h>
#include <bitset>

namespace vlx
{
	class CollisionLayer
	{
	public:
		constexpr operator std::size_t() const;

		constexpr void Add(std::size_t layer);
		constexpr void Remove(std::size_t layer);

		constexpr bool Has(std::size_t layer);
		constexpr bool HasAny(std::size_t layers);

	private:
		std::size_t m_layer {~std::size_t(0)}; // collide with all by default
	};

	constexpr CollisionLayer::operator std::size_t() const
	{
		return m_layer;
	}

	constexpr void CollisionLayer::Add(std::size_t layer)
	{
		m_layer |= layer;
	}
	constexpr void CollisionLayer::Remove(std::size_t layer)
	{
		m_layer &= ~layer;
	}

	constexpr bool CollisionLayer::Has(std::size_t layer)
	{
		return (m_layer & layer) == layer;
	}

	constexpr bool CollisionLayer::HasAny(std::size_t layers)
	{
		return (m_layer & layers) != 0;
	}
}