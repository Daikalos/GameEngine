#pragma once

#include "Shape.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class Circle final : public Shape
	{
	public:
		constexpr Circle();
		constexpr Circle(const float radius);

	public:
		constexpr float GetRadius() const noexcept;
		constexpr float GetRadiusSqr() const noexcept;

		constexpr void SetRadius(float radius);

	public:
		constexpr auto GetType() const noexcept -> Type override;

		VELOX_API void AdjustBody(PhysicsBody& body) const override;
		VELOX_API Vector2f ComputeCenter(const Vector2f& position) const override;
		VELOX_API RectFloat ComputeAABB(const GlobalTransform& transform) const override;

	private:
		float m_radius		{16.0f};
		float m_radius_sqr	{256.0f};
	};

	constexpr Circle::Circle() {};

	constexpr Circle::Circle(const float radius) 
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