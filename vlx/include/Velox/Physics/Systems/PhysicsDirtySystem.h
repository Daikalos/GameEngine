#pragma once

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/TransformMatrix.h>

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
		using DirtyLocalSystem	= System<Collider, Transform>;
		using CircleSystem		= System<Circle, Collider, Transform>;
		using BoxSystem			= System<Box, Collider, Transform, TransformMatrix>;
		using PointSystem		= System<Point, Collider, Transform>;
		using PolySystem		= System<Polygon, Collider, Transform, TransformMatrix>;

	public:
		PhysicsDirtySystem(EntityAdmin& entity_admin, LayerType id);

	public:
		void FixedUpdate() override;

	private:
		DirtyLocalSystem	m_dirty_transform;
		DirtyLocalSystem	m_dirty_physics;

		CircleSystem		m_circles;
		BoxSystem			m_boxes;
		PointSystem			m_points;
		PolySystem			m_polygons;
	};
}