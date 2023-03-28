#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../CollisionObject.h"
#include "../CollisionResult.h"
#include "../CollisionData.h"
#include "../CollisionTable.h"
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
		void UpdateData(Shape& shape, const Transform& transform);

	private:
		DirtyGlobalSystem	m_dirty_transform;
		DirtyLocalSystem	m_dirty_physics;

		CircleSystem		m_circles_aabb;
		BoxSystem			m_boxes_aabb;
	};
}