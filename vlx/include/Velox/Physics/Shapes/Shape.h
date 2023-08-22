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
		constexpr Shape(); // for some weird reason, constexpr constructors cannot be defaulted
		constexpr virtual ~Shape() = default; // may not be needed

	public:
		virtual auto GetTypeV() const noexcept -> Type = 0;

		constexpr float GetRadius() const noexcept;
		constexpr float GetRadiusSqr() const noexcept;

	protected:
		virtual void AdjustBody(PhysicsBody& body) const = 0;

	protected:
		float m_radius;		// radius for shape, P_POLYGON_RADIUS for polygons
		float m_radius_sqr;

		friend class PhysicsDirtySystem;
	};

	constexpr Shape::Shape() : m_radius(P_POLYGON_RADIUS), m_radius_sqr(au::Sqr(P_POLYGON_RADIUS)) {}

	constexpr float Shape::GetRadius() const noexcept		{ return m_radius; }
	constexpr float Shape::GetRadiusSqr() const noexcept	{ return m_radius_sqr; }
}