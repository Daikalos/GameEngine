#pragma once

#include <vector>
#include <unordered_set>

#include <Velox/ECS.hpp>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include <Velox/Utility/ContainerUtils.h>

#include "../CollisionObject.h"
#include "../CollisionResult.h"
#include "../CollisionArbiter.h"
#include "../CollisionTable.h"
#include "../Collider.h"
#include "../PhysicsBody.h"

#include "BroadSystem.h"

namespace vlx
{
	class VELOX_API NarrowSystem final
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

		using CollisionArbiters = std::vector<CollisionArbiter>;

	private:
		struct CollisionEventPair
		{
			using ColliderRef = ComponentRef<Collider>;

			CollisionEventPair(ColliderRef&& lcoll, ColliderRef&& rcoll) :
				lhs_collider(lcoll), rhs_collider(rcoll), 
				lhs_eid(lcoll.GetEntityID()), rhs_eid(rcoll.GetEntityID()) {}

			CollisionEventPair(EntityID leid, EntityID reid) :
				lhs_eid(leid), rhs_eid(reid) {}

			CollisionEventPair(const EntityPair& pair) :
				lhs_eid(pair.first), rhs_eid(pair.second) {}

			ColliderRef	lhs_collider;
			ColliderRef	rhs_collider;

			EntityID lhs_eid {NULL_ENTITY};
			EntityID rhs_eid {NULL_ENTITY};

			bool operator==(const CollisionEventPair& cp) const
			{
				return lhs_eid == cp.lhs_eid && rhs_eid == cp.rhs_eid;
			}
		};

		struct CollisionEventPairHash
		{
			constexpr std::size_t operator()(const CollisionEventPair& cp) const
			{
				return cu::PairIntegerHash<std::size_t>()({ cp.lhs_eid, cp.rhs_eid });
			}
		};

	public:
		NarrowSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void Update(BroadSystem& broad);

	public:
		auto GetArbiters() const noexcept -> const CollisionArbiters&;
		auto GetArbiters() noexcept -> CollisionArbiters&;

	private:
		void CheckCollision(const CollisionObject& lhs, const CollisionObject& rhs);

	private:
		EntityAdmin* m_entity_admin	{nullptr};

		CollisionArbiters		m_arbiters;

		std::vector<EntityPair> m_curr_collisions;
		std::vector<EntityPair> m_prev_collisions;
		std::vector<EntityPair> m_difference;

		std::unordered_set<CollisionEventPair, CollisionEventPairHash> m_collisions;
	};
}