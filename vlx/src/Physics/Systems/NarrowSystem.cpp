#include <Velox/Physics/Systems/NarrowSystem.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system)
	: SystemAction(entity_admin, id),

	m_broad_system(&broad_system),

	m_initialize_collisions(entity_admin, id - 1),
	m_exit_collisions(entity_admin, id + 1)
{

}

constexpr bool NarrowSystem::IsRequired() const noexcept
{
	return true;
}

void NarrowSystem::PreUpdate()
{

}

void NarrowSystem::Update()
{

}

void NarrowSystem::FixedUpdate()
{
	m_collision_data.clear();

	for (const auto i : m_broad_system->m_collision_indices)
	{
		auto& pair = m_broad_system->m_collision_pairs[i];

		CollisionObject& A = pair.first;
		CollisionObject& B = pair.second;

		CollisionData data(&A, &B);
		CollisionTable::Collide(data, A, B);

		if (data.contact_count)
		{
			m_collision_data.emplace_back(data);

			//CollisionResult first_result(
			//	0, 0, 
			//	data.normal, 
			//	data.contacts[0], 
			//	data.penetration);

			//CollisionResult second_result(
			//	0, 0,
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

void NarrowSystem::PostUpdate()
{

}

void NarrowSystem::CallEvents(const CollisionData& data, const CollisionObject& object)
{

}
