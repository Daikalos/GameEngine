#pragma once

#include <stdint.h>
#include <bitset>

namespace vlx
{
	class CollisionLayer
	{
	public:
		constexpr operator size_t() const;

		constexpr void Add(size_t layer);
		constexpr void Remove(size_t layer);

		constexpr bool Has(size_t layer);
		constexpr bool HasAny(size_t layers);

	private:
		size_t m_layer {~size_t(0)}; // collide with all by default
	};

	constexpr CollisionLayer::operator size_t() const
	{
		return m_layer;
	}

	constexpr void CollisionLayer::Add(size_t layer)
	{
		m_layer |= layer;
	}
	constexpr void CollisionLayer::Remove(size_t layer)
	{
		m_layer &= ~layer;
	}

	constexpr bool CollisionLayer::Has(size_t layer)
	{
		return (m_layer & layer) == layer;
	}

	constexpr bool CollisionLayer::HasAny(size_t layers)
	{
		return (m_layer & layers) != 0;
	}
}