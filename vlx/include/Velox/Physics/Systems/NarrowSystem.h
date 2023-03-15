#pragma once

#include <vector>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../CollisionObject.h"
#include "../CollisionResult.h"
#include "../CollisionData.h"
#include "../CollisionTable.h"
#include "../Collision.h"
#include "../PhysicsBody.h"

#include "PhysicsSystem.h"

namespace vlx
{
	class VELOX_API NarrowSystem final : public SystemAction
	{
	private:
		using CollisionSystem = System<Collision, LocalTransform, Transform>;

	public:
		NarrowSystem(EntityAdmin& entity_admin, const LayerType id, PhysicsSystem& physics_system);

	public:
		NODISC constexpr bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		PhysicsSystem* m_physics_system {nullptr};
	};
}