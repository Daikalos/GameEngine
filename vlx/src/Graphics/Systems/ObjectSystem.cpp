#include <Velox/Graphics/Systems/ObjectSystem.h>

using namespace vlx;

ObjectSystem::ObjectSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), m_object_system(entity_admin, id)
{
	m_object_system.All([this](std::span<const EntityID> entities, Object* objects)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (!objects[i].IsAlive)
					DeleteObjectDelayed(entities[i]);
			}
		});
}

constexpr bool ObjectSystem::IsRequired() const noexcept
{
	return true;
}

Entity ObjectSystem::CreateObject() const
{
	return Entity(*m_entity_admin);
}

void ObjectSystem::DeleteObjectDelayed(const EntityID entity_id)
{
	m_commands.emplace(DeleteEntity(entity_id), DEL_ENTITY);
}
void ObjectSystem::DeleteObjectInstant(const EntityID entity_id)
{
	m_entity_admin->RemoveEntity(entity_id);
}

void ObjectSystem::PreUpdate()
{

}

void ObjectSystem::Update()
{
	Execute();
}

void ObjectSystem::FixedUpdate()
{

}

void ObjectSystem::PostUpdate()
{
	while (!m_commands.empty())
	{
		const auto& pair = m_commands.front();
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

		m_commands.pop();
	}
}
