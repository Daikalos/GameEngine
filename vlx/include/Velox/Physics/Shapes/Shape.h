#pragma once

#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Vector2.hpp>

#include <Velox/Config.hpp>

#include "../PhysicsBody.h"

namespace vlx
{
	class VELOX_API Shape
	{
	public:
		enum Type : short
		{
			None = -1,
			Circle,
			Box,
			Point,
			Convex,

			// considering adding line, triangle and capsule in the future

			Count // always keep last
		};

	public:
		constexpr virtual ~Shape() = default; // may not be needed

	public:
		float GetRadius() const noexcept;
		float GetRadiusSqr() const noexcept;

	public:
		constexpr virtual auto GetType() const noexcept -> Shape::Type = 0;

	protected:
		virtual void AdjustBody(PhysicsBody& body) const = 0;

	protected:
		float m_radius		{P_POLYGON_RADIUS};			// radius for shape, P_POLYGON_RADIUS for polygons
		float m_radius_sqr	{au::Sqr(P_POLYGON_RADIUS)};

		friend class PhysicsDirtySystem;
	};
}