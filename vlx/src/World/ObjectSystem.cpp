#include <Velox/World/ObjectSystem.h>

using namespace vlx;

ObjectSystem::ObjectSystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id)
{

}

bool ObjectSystem::IsRequired() const noexcept
{
	return true;
}

Entity ObjectSystem::CreateEntity() const
{
	return Entity(*m_entity_admin);
}

void ObjectSystem::RemoveEntity(EntityID entity_id, ExecutionStage stage)
{
	if (stage == S_Instant)
	{
		m_entity_admin->RemoveEntity(entity_id);
		return;
	}

	m_command_table[stage].emplace(std::in_place_index<4>, entity_id);
}

void ObjectSystem::ExecuteManually()
{
	ExecuteCommands(S_Manual);
}

void ObjectSystem::Start()
{
	m_entity_admin->RunSystems(LYR_OBJECTS_START);
}

void ObjectSystem::PreUpdate()
{
	ExecuteCommands(S_PreUpdate);
	m_entity_admin->RunSystems(LYR_OBJECTS_PRE);
}

void ObjectSystem::Update()
{
	ExecuteCommands(S_Update);
	m_entity_admin->RunSystems(LYR_OBJECTS_UPDATE);
}

void ObjectSystem::FixedUpdate()
{
	ExecuteCommands(S_FixedUpdate);
	m_entity_admin->RunSystems(LYR_OBJECTS_FIXED);
}

void ObjectSystem::PostUpdate()
{
	ExecuteCommands(S_PostUpdate);
	m_entity_admin->RunSystems(LYR_OBJECTS_POST);
}

void ObjectSystem::ExecuteCommands(ExecutionStage stage)
{
	auto& commands = m_command_table[stage];
	while (!commands.empty())
	{
		VisitCommand(commands.front());
		commands.pop();
	}
}

void ObjectSystem::VisitCommand(const Command& command)
{
	std::visit(traits::Overload
		{
			[this](const AddCompData& data)
			{
				m_entity_admin->AddComponent(data.entity_id, data.component_id);
			},
			[this](const AddCompsData& data)
			{
				m_entity_admin->AddComponents(data.entity_id, data.component_ids, data.archetype_id);
			},
			[this](const RmvCompData& data)
			{
				m_entity_admin->RemoveComponent(data.entity_id, data.component_id);
			},
			[this](const RmvCompsData& data)
			{
				m_entity_admin->RemoveComponents(data.entity_id, data.component_ids, data.archetype_id);
			},
			[this](const RmvEntityData& data)
			{
				m_entity_admin->RemoveEntity(data.entity_id);
			}
		}, command);
}
