#pragma once

#include "Shape.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API Circle final : public Shape
	{
	public:
		Circle() = default;
		Circle(const float radius);

	public:
		constexpr Type GetType() const noexcept override;

		constexpr float GetRadius() const noexcept;
		constexpr float GetRadiusSqr() const noexcept;

		constexpr void SetRadius(float radius);

	public:
		void Initialize(PhysicsBody& body) const override;
		RectFloat GetAABB() const override;

	private:
		float m_radius		{16.0f};
		float m_radius_sqr	{256.0f};
	};
}