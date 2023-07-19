#pragma once

#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Vector2.hpp>
#include <Velox/System/Rot2f.h>
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
		const RectFloat& GetAABB() const noexcept;
		const Vector2f& GetCenter() const noexcept;;
		float GetRadius() const noexcept;
		float GetRadiusSqr() const noexcept;

	public:
		constexpr virtual auto GetType() const noexcept -> Shape::Type = 0;

	protected:
		virtual void AdjustBody(PhysicsBody& body) const = 0;

	private:
		void UpdateAABB(const RectFloat& aabb);
		void UpdateCenter(const Vector2f& center);

	protected:
		RectFloat	m_aabb;		// aabb for queries
		Vector2f	m_center;

		float		m_radius	 {P_POLYGON_RADIUS};			// radius for shape, P_POLYGON_RADIUS for polygons
		float		m_radius_sqr {au::Sqr(P_POLYGON_RADIUS)};

		friend class PhysicsDirtySystem;
	};

	class VELOX_API ShapeRotatable : public Shape
	{
	public:
		const Rot2f& GetOrientation() const;

	private:
		void UpdateOrientation(sf::Angle angle);

	private:
		mutable Rot2f	m_orientation;		
		sf::Angle		m_angle;			
		mutable bool	m_update {true};

		friend class PhysicsDirtySystem;
	};
}