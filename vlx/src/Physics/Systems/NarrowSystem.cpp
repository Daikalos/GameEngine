#include <Velox/Physics/Systems/NarrowSystem.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) 
	: m_broad_system(&broad_system), m_initialize_collisions(entity_admin, id - 1), m_exit_collisions(entity_admin, id + 1)
{

}

void NarrowSystem::Update(
	std::span<typename BroadSystem::CollisionPair> pairs,
	std::span<typename BroadSystem::CollisionIndex> indices)
{
	m_arbiters.clear();

	for (const auto i : indices)
	{
		auto& pair = pairs[i];

		CollisionObject& A = pair.first;
		CollisionObject& B = pair.second;

		CollisionArbiter arbiter(&A, &B);
		CollisionTable::Collide(arbiter, *A.shape, A.type, *B.shape, B.type);

		if (arbiter.contacts_count)
		{
			m_arbiters.emplace_back(arbiter);

			//CollisionResult first_result(
			//	A.entity_id, B.entity_id,
			//	data.normal,
			//	data.contacts[0],
			//	data.penetration);

			//CollisionResult second_result(
			//	B.entity_id, A.entity_id,
			//	-data.normal,		// flip normal
			//	data.contacts[0],
			//	data.penetration);

			//if (!pair.first.collision->colliding)
			//{
			//	pair.first.collision->OnEnter(first_result);
			//	pair.first.collision->colliding = true;
			//}

			//pair.first.collision->OnOverlap(first_result);
			//pair.second.collision->OnOverlap(second_result);
		}
	}
}

std::span<CollisionArbiter> NarrowSystem::GetArbiters() noexcept
{
	return m_arbiters;
}

std::span<const CollisionArbiter> NarrowSystem::GetArbiters() const noexcept
{
	return m_arbiters;
}

void NarrowSystem::CallEvents(const CollisionArbiter& arbiter, const CollisionObject& object)
{

}
