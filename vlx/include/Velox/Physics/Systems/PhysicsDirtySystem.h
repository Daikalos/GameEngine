#pragma once

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../Collision.h"

namespace vlx
{
	class VELOX_API PhysicsDirtySystem final : public SystemAction
	{
	public:
		using DirtyGlobalSystem = System<Collision, Transform>;
		using DirtyLocalSystem	= System<Collision, LocalTransform>;
		using CircleSystem		= System<Circle, Collision, Transform>;
		using BoxSystem			= System<Box, Collision, Transform>;
		using PointSystem		= System<Point, Collision, Transform>;

	public:
		PhysicsDirtySystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		DirtyGlobalSystem	m_dirty_transform;
		DirtyLocalSystem	m_dirty_physics;

		CircleSystem		m_circles_aabb;
		BoxSystem			m_boxes_aabb;
		PointSystem			m_points_aabb;
	};
}