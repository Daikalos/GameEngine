#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_TRANSFORM)
{
	m_system.Action([this](std::span<const EntityID> entities, Transform* transforms, Relation* relations)
		{
			Update(relations);
		});
}

void TransformSystem::Update(Relation* relations)
{
	while (!m_attachments.empty())
	{

	}
}