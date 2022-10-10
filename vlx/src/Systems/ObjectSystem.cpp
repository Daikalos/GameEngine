#include <Velox/Systems/ObjectSystem.h>

using namespace vlx;

ObjectSystem::ObjectSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_OBJECTS)
{
	m_system.Action([this](std::span<const EntityID> entities, GameObject* gameobjects)
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
	m_command_queue.push(std::make_pair(DeleteEntity(entity_id), DEL_ENTITY));
}
void ObjectSystem::DeleteObjectInstant(const EntityID entity_id)
{
	m_entity_admin->RemoveEntity(entity_id);
}

void ObjectSystem::Update()
{
	while (!m_command_queue.empty())
	{
		const auto& pair = m_command_queue.front();
		const CommandType command = pair.second;

		switch (command)
		{
		case ADD_COMPONENT:
			{
				const auto& add_cmp = std::get<AddComponent>(pair.first);
				m_entity_admin->AddComponent(add_cmp.entity_id, add_cmp.component_id);
			}
			break;
		case DEL_ENTITY:
			{
				const auto& del_ent = std::get<DeleteEntity>(pair.first);
				m_entity_admin->RemoveEntity(del_ent.entity_id);
			}
			break;
		case DEL_COMPONENT:
			{
				const auto& del_cmp = std::get<DeleteComponent>(pair.first);
				m_entity_admin->RemoveComponent(del_cmp.entity_id, del_cmp.component_id);
			}
			break;
		}

		m_command_queue.pop();
	}
}