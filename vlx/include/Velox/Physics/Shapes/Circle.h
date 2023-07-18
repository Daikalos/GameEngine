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
		constexpr explicit Circle(float radius);

	public:
		constexpr void SetRadius(float radius);

	public:
		constexpr auto GetType() const noexcept -> Type override;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;

	public:
		VELOX_API RectFloat ComputeAABB(const Transform& transform) const;
	};

	constexpr Circle::Circle() 
	{
		SetRadius(16.0f);
	};

	constexpr Circle::Circle(float radius) 
	{
		SetRadius(radius);
	}

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