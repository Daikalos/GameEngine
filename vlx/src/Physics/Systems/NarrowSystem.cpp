#include <Velox/Physics/Systems/NarrowSystem.h>

#include <Velox/ECS/EntityAdmin.h>

#include <Velox/System/SimpleTransform.h>

#include <Velox/Physics/Collision/CollisionTable.h>
#include <Velox/Physics/Collision/WorldManifold.h>
#include <Velox/Physics/Collision/CollisionBody.h>

#include <Velox/Physics/Collider/ColliderEvents.h>

#include <Velox/Physics/BodyTransform.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin) 
	: m_entity_admin(&entity_admin) {}

void NarrowSystem::Update(BroadSystem& broad)
{
	m_collisions.clear();
	m_manifolds.clear();

	for (const auto& pair : broad.GetCollisions())
		CheckCollision(broad, pair.first, pair.second);

	const auto cmp = [](const EntityPair& lhs, const EntityPair& rhs)
	{
		return (lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second);
	};

	std::ranges::sort(m_curr_collisions, cmp); // sort to quickly allow for finding the differences between previous and current

	std::ranges::set_difference(
		m_prev_collisions, m_curr_collisions, std::back_inserter(m_difference), cmp);

	for (const EntityPair& pair : m_difference)
	{
		const auto it = m_collision_map.find(pair);
		if (it != m_collision_map.end())
		{
			if (it->lhs_exit) it->lhs_exit->OnExit(it->rhs_eid);
			if (it->rhs_exit) it->rhs_exit->OnExit(it->lhs_eid);

			m_collision_map.erase(it);
		}
	}

	m_prev_collisions = m_curr_collisions;
	m_curr_collisions.clear();

	m_difference.clear();
}

auto NarrowSystem::GetCollisions() const noexcept -> const CollisionList&
{
	return m_collisions;
}
auto NarrowSystem::GetCollisions() noexcept -> CollisionList&
{
	return m_collisions;
}

auto NarrowSystem::GetManifolds() const noexcept -> const LocalManifolds&
{
	return m_manifolds;
}
auto NarrowSystem::GetManifolds() noexcept -> LocalManifolds&
{
	return m_manifolds;
}

void NarrowSystem::CheckCollision(BroadSystem& broad, uint32 l, uint32 r)
{
	const CollisionBody& A = broad.GetBody(l);
	const CollisionBody& B = broad.GetBody(r);

	SimpleTransform AW;
	SimpleTransform BW;

	AW.SetPosition(A.transform->GetPosition());
	BW.SetPosition(B.transform->GetPosition());

	AW.SetRotation(A.transform->GetRotation());
	BW.SetRotation(B.transform->GetRotation());

	LocalManifold lm = CollisionTable::Collide(*A.shape, AW, A.type, *B.shape, BW, B.type);

	if (lm.contacts_count) // successfully collided if not zero
	{
		PhysicsBody* AB = A.body;
		PhysicsBody* BB = B.body;

		if (AB && BB && (AB->GetType() == BodyType::Dynamic || BB->GetType() == BodyType::Dynamic)) // only resolve if both entities has a physics body and either one is dynamic
		{
			m_collisions.emplace_back(l, r);
			m_manifolds.emplace_back(lm);
		}

		const bool has_enter	= (A.enter && A.enter->OnEnter) || (B.enter && B.enter->OnEnter);
		const bool has_exit		= (A.exit && A.exit->OnExit) || (B.exit && B.exit->OnExit);
		const bool has_overlap	= (A.overlap && A.overlap->OnOverlap);

		if (has_enter || has_exit || has_overlap)
		{
			EntityPair pair = (A.entity_id < B.entity_id) ?
				EntityPair{A.entity_id, B.entity_id} :
				EntityPair{B.entity_id, A.entity_id};

			CollisionResult a_result{B.entity_id}; // store other entity
			CollisionResult b_result{A.entity_id};

			WorldManifold world;
			world.Initialize(lm, AW, A.shape->GetRadius(), BW, B.shape->GetRadius());

			for (uint8 i = 0; i < lm.contacts_count; ++i)
			{
				a_result.contacts[i].hit			= world.contacts[i];
				a_result.contacts[i].penetration	= world.penetrations[i];
			}

			b_result = a_result;

			a_result.normal =  world.normal;
			b_result.normal = -world.normal; // flip normal for other

			if (has_enter || has_exit)
			{
				if (auto ait = m_collision_map.find(pair); ait == m_collision_map.end()) // first time
				{
 					if (A.enter) A.enter->OnEnter(a_result);
					if (B.enter) B.enter->OnEnter(b_result);

					m_collision_map.emplace(
						m_entity_admin->GetComponentRef(A.entity_id, A.exit),
						m_entity_admin->GetComponentRef(B.entity_id, B.exit), A.entity_id, B.entity_id);
				}

				m_curr_collisions.emplace_back(pair);
			}

			if (has_overlap)
				A.overlap->OnOverlap(a_result); // only needs to be called for A since B overlap will be called later
		}
	}
}
