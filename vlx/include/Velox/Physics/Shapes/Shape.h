#pragma once

#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Mat2f.hpp>
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
		constexpr virtual ~Shape() = default; // i dont think this is needed

	public:
		const Mat2f& GetOrientation() const;
		const RectFloat& GetAABB() const;
		const Vector2f& GetCenter() const;

	public:
		constexpr virtual auto GetType() const noexcept -> Shape::Type = 0;

	protected:
		virtual void AdjustBody(PhysicsBody& body) const = 0;
		virtual Vector2f ComputeCenter(const Vector2f& position) const = 0;
		virtual RectFloat ComputeAABB(const GlobalTransform& transform) const = 0;

	private:
		void UpdateOrientation(sf::Angle angle);
		void UpdateAABB(const RectFloat& aabb);
		void UpdateCenter(const Vector2f& center);

	protected:
		mutable Mat2f	m_orientation;		// rotation matrix
		RectFloat		m_aabb;				// aabb for queries
		Vector2f		m_center;			// center of shape
		sf::Angle		m_angle;			// cache orientation
		mutable bool	m_update {true};	// whether to update orientation

		friend class PhysicsDirtySystem;
	};
}