#include <Velox/Physics/Systems/NarrowSystem.h>

#include <Velox/ECS/EntityAdmin.h>

#include <Velox/System/SimpleTransform.h>
#include <Velox/Physics/WorldManifold.h>

#include <Velox/Physics/CollisionTable.h>
#include <Velox/Physics/ColliderEvents.h>
#include <Velox/Physics/CollisionBody.h>
#include <Velox/Physics/BodyTransform.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin) 
	: m_entity_admin(&entity_admin) {}

void NarrowSystem::Update(BroadSystem& broad)
{
	m_arbiters.clear();

	for (const auto& pair : broad.GetCollisions())
	{
		CheckCollision(
			broad.GetBody(pair.first),
			broad.GetBody(pair.second));
	}

	const auto cmp = [](const EntityPair& lhs, const EntityPair& rhs)
	{
		return (lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second);
	};

	std::ranges::sort(
		m_curr_collisions.begin(), m_curr_collisions.end(), cmp); // sort to quickly allow for finding the differences between previous and current

	std::ranges::set_difference(
		m_prev_collisions.begin(), m_prev_collisions.end(),
		m_curr_collisions.begin(), m_curr_collisions.end(), std::inserter(m_difference, m_difference.begin()), cmp);

	for (const EntityPair& pair : m_difference)
	{
		const auto it = m_collisions.find(pair);
		if (it != m_collisions.end())
		{
			if (it->lhs_exit) it->lhs_exit->OnExit(it->rhs_eid);
			if (it->rhs_exit) it->rhs_exit->OnExit(it->lhs_eid);

			m_collisions.erase(it);
		}
	}

	m_prev_collisions = m_curr_collisions;
	m_curr_collisions.clear();

	m_difference.clear();
}

auto NarrowSystem::GetArbiters() const noexcept -> const CollisionArbiters&
{
	return m_arbiters;
}

auto NarrowSystem::GetArbiters() noexcept -> CollisionArbiters&
{
	return m_arbiters;
}

void NarrowSystem::CheckCollision(CollisionBody& A, CollisionBody& B)
{
	CollisionArbiter arbiter;
	CollisionTable::Collide(arbiter, *A.shape, A.type, *B.shape, B.type);

	if (arbiter.manifold.contacts_count) // successfully collided if not zero
	{
		PhysicsBody* AB = A.body;
		PhysicsBody* BB = B.body;

		if (AB && BB && (AB->GetType() == BodyType::Dynamic || BB->GetType() == BodyType::Dynamic)) // only resolve if both entities has a physics body and either is dynamic
		{
			arbiter.A = &A;
			arbiter.B = &B;

			m_arbiters.emplace_back(arbiter);
		}

		const bool has_enter	= (A.enter && A.enter->OnEnter) || (B.enter && B.enter->OnEnter);
		const bool has_exit		= (A.exit && A.exit->OnExit) || (B.exit && B.exit->OnExit);
		const bool has_overlap	= (A.overlap && A.overlap->OnOverlap);

		if (has_enter || has_exit || has_overlap)
		{
			EntityPair pair = (A.entity_id < B.entity_id) ?
				EntityPair(A.entity_id, B.entity_id) :
				EntityPair(B.entity_id, A.entity_id);

			CollisionResult a_result{B.entity_id}; // store other entity
			CollisionResult b_result{A.entity_id};

			SimpleTransform AW;
			SimpleTransform BW;

			AW.SetRotation(A.transform->GetRotation());
			BW.SetRotation(B.transform->GetRotation());

			AW.SetPosition(A.transform->GetPosition());
			BW.SetPosition(B.transform->GetPosition());

			WorldManifold world;
			world.Initialize(arbiter.manifold, AW, A.shape->GetRadius(), BW, B.shape->GetRadius());

			for (uint8 i = 0; i < arbiter.manifold.contacts_count; ++i)
			{
				a_result.contacts[i].hit			= world.contacts[i];
				a_result.contacts[i].penetration	= world.penetrations[i];
			}

			b_result = a_result;

			a_result.normal =  world.normal;
			b_result.normal = -world.normal; // flip normal for other

			if (has_enter || has_exit)
			{
				if (auto ait = m_collisions.find(pair); ait == m_collisions.end()) // first time
				{
 					if (A.enter) A.enter->OnEnter(a_result);
					if (B.enter) B.enter->OnEnter(b_result);

					m_collisions.emplace(
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
