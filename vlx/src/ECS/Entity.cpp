#include <Velox/ECS/Entity.h>

using namespace vlx;

Entity::Entity(EntityAdmin& entity_admin)
	: m_id(entity_admin.GetNewId()), m_entity_admin(&entity_admin)
{
	m_entity_admin->RegisterEntity(m_id);
}

Entity::~Entity()
{
	m_entity_admin->RemoveEntity(m_id);
}

constexpr EntityID Entity::GetId() const
{
	return m_id;
}