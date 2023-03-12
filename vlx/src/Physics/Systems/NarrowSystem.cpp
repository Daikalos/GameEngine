#include <Velox/Physics/Systems/NarrowSystem.h>

using namespace vlx;

NarrowSystem::NarrowSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id)
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

}

void NarrowSystem::PostUpdate()
{

}
