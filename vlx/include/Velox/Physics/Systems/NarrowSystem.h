#pragma once

#include <vector>
#include <unordered_set>

#include "BroadSystem.h"

#include "../Collision/CollisionArbiter.h"
#include "../Collision/LocalManifold.h"

namespace vlx
{
	class EntityAdmin;
	class CollisionBody;
	struct ColliderExit;

	class VELOX_API NarrowSystem final
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

		using CollisionArbiters = std::vector<CollisionArbiter>;
		using LocalManifolds	= std::vector<LocalManifold>;

	private:
		struct CollisionEventPair
		{
			using ExitRef = ComponentRef<ColliderExit>;

			CollisionEventPair(ExitRef&& lexit, ExitRef&& rexit, EntityID leid, EntityID reid) :
				lhs_exit(lexit), rhs_exit(rexit), lhs_eid(leid), rhs_eid(reid) {}

			CollisionEventPair(EntityID leid, EntityID reid) :
				lhs_eid(leid), rhs_eid(reid) {}

			CollisionEventPair(const EntityPair& pair) :
				lhs_eid(pair.first), rhs_eid(pair.second) {}

			ExitRef	lhs_exit;
			ExitRef	rhs_exit;

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
		NarrowSystem(EntityAdmin& entity_admin);

	public:
		void Update(BroadSystem& broad);

	public:
		auto GetArbiters() const noexcept -> const CollisionArbiters&;
		auto GetArbiters() noexcept -> CollisionArbiters&;

		auto GetManifolds() const noexcept -> const LocalManifolds&;
		auto GetManifolds() noexcept -> LocalManifolds&;

	private:
		void CheckCollision(CollisionBody& A, CollisionBody& B);

	private:
		EntityAdmin*			m_entity_admin	{nullptr};

		CollisionArbiters		m_arbiters;
		LocalManifolds			m_manifolds;

		std::vector<EntityPair> m_curr_collisions;
		std::vector<EntityPair> m_prev_collisions;
		std::vector<EntityPair> m_difference;

		std::unordered_set<CollisionEventPair, CollisionEventPairHash> m_collisions;
	};
}