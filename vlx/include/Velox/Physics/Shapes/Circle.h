#pragma once

#include "Shape.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API Circle final : public ShapeCRTP<Circle>
	{
	public:
		Circle() = default;
		Circle(const float radius);

	public:
		constexpr float GetRadius() const noexcept;
		constexpr float GetRadiusSqr() const noexcept;

		constexpr void SetRadius(float radius);

	public:
		constexpr auto GetType() const noexcept -> Type;

		void InitializeImpl(PhysicsBody& body) const;
		void UpdateAABBImpl(const Transform& transform);

	private:
		float m_radius		{16.0f};
		float m_radius_sqr	{256.0f};
	};
}