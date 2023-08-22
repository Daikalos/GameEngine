#pragma once

#include <Velox/System/Vector2.hpp>
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
		static consteval auto GetType() noexcept -> Type;

		VELOX_API auto GetTypeV() const noexcept -> Type override;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;

	public:
		VELOX_API RectFloat ComputeAABB(const Vector2f& position) const;
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

	consteval auto Circle::GetType() noexcept -> Type
	{
		return Type::Circle;
	}
}