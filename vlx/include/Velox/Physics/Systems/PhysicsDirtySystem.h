#pragma once

#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../Collider.h"

namespace vlx
{
	class VELOX_API PhysicsDirtySystem final : public SystemAction
	{
	public:
		using DirtyGlobalSystem = System<Collider, GlobalTransform>;
		using DirtyLocalSystem	= System<Collider, Transform>;
		using CircleSystem		= System<Circle, Collider, GlobalTransform>;
		using BoxSystem			= System<Box, Collider, GlobalTransform>;
		using PointSystem		= System<Point, Collider, GlobalTransform>;

	public:
		PhysicsDirtySystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		bool IsRequired() const noexcept override;

	public:
		void Start() override;
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