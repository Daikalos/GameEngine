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
#include "../CollisionArbiter.h"
#include "../CollisionTable.h"
#include "../Collision.h"
#include "../PhysicsBody.h"

#include "BroadSystem.h"

namespace vlx
{
	class VELOX_API NarrowSystem final
	{
	private:
		using CollisionSystem = System<Collision, LocalTransform, Transform>;

	public:
		NarrowSystem(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system);

	public:
		void Update(
			std::span<typename BroadSystem::CollisionPair> pairs,
			std::span<typename BroadSystem::CollisionIndex> indices);

	public:
		std::span<CollisionArbiter> GetArbiters() noexcept;
		std::span<const CollisionArbiter> GetArbiters() const noexcept;

	private:
		void CallEvents(const CollisionArbiter& data, const CollisionObject& object);

	private:
		BroadSystem* m_broad_system {nullptr};

		std::vector<CollisionArbiter> m_arbiters;

		System<PhysicsBody> m_initialize_collisions;
		System<PhysicsBody> m_exit_collisions;
	};
}