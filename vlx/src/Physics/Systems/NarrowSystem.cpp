#include <Velox/Physics/Systems/NarrowSystem.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin, const LayerType id, PhysicsSystem& physics_system)
	: SystemAction(entity_admin, id), m_physics_system(&physics_system)
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
	Execute();
}

void NarrowSystem::PostUpdate()
{

}
