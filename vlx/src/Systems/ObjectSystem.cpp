#include <Velox/Systems/ObjectSystem.h>

using namespace vlx;

ObjectSystem::ObjectSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_OBJECTS)
{
	m_system.Action([this](const EntityAdmin& entity_admin, Time& time, 
		std::span<const EntityID> entities, GameObject* gameobjects)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (!gameobjects[i].is_alive)
					DeleteObjectDelayed(entities[i]);
			}

			Update();
		});
}

Entity ObjectSystem::CreateObject() const
{
	return Entity(*m_entity_admin);
}

void ObjectSystem::DeleteObjectDelayed(const EntityID entity_id)
{
	m_deletion_queue.push(std::make_pair(DeleteEntity(entity_id), DEL_ENTITY));
}
void ObjectSystem::DeleteObjectInstant(const EntityID entity_id)
{
	m_entity_admin->RemoveEntity(entity_id);
}

void ObjectSystem::Update()
{
	while (!m_deletion_queue.empty())
	{
		const auto& pair = m_deletion_queue.front();
		const CommandType command = pair.second;

		switch (command)
		{
		case DEL_ENTITY:
			m_entity_admin->RemoveEntity(std::get<DeleteEntity>(pair.first).entity_id);
			break;
		case DEL_COMPONENT:
			const auto& del_cmp = std::get<DeleteComponent>(pair.first);
			m_entity_admin->RemoveComponent(del_cmp.entity_id, del_cmp.component_id);
			break;
		}

		m_deletion_queue.pop();
	}
}