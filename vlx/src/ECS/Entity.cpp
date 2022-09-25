#include <Velox/ECS/Entity.h>

using namespace vlx;

Entity::Entity(EntityAdmin& entity_admin)
	: m_id(entity_admin.GetNewId()), m_entity_admin(&entity_admin)
{
	m_entity_admin->RegisterEntity(m_id);
}
Entity::Entity(Entity&& entity) noexcept
	: m_id(entity.m_id), m_entity_admin(entity.m_entity_admin)
{
	entity.m_id = NULL_ENTITY;
	entity.m_entity_admin = nullptr;
}

Entity::~Entity()
{
	if (m_id != NULL_ENTITY && m_entity_admin)
		m_entity_admin->RemoveEntity(m_id);
}

constexpr EntityID Entity::GetId() const
{
	return m_id;
}