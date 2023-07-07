#pragma once

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Circle final : public Shape
	{
	public:
		constexpr Circle();
		constexpr Circle(float radius);

	public:
		constexpr float GetRadius() const noexcept;
		constexpr float GetRadiusSqr() const noexcept;

		constexpr void SetRadius(float radius);

	public:
		constexpr auto GetType() const noexcept -> Type override;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;

	public:
		VELOX_API RectFloat ComputeAABB(const Transform& transform) const;

	private:
		float m_radius		{16.0f};
		float m_radius_sqr	{256.0f};
	};

	constexpr Circle::Circle() {};

	constexpr Circle::Circle(float radius) 
		: m_radius(radius), m_radius_sqr(au::Sqr(radius)) {}

	constexpr float Circle::GetRadius() const noexcept		{ return m_radius; }
	constexpr float Circle::GetRadiusSqr() const noexcept	{ return m_radius_sqr; }

	constexpr void Circle::SetRadius(float radius)
	{
		m_radius = radius;
		m_radius_sqr = au::Sqr(radius);
	}

	constexpr auto Circle::GetType() const noexcept -> Type
	{
		return Type::Circle;
	}
}