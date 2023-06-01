#include <Velox/World/ObjectSystem.h>

using namespace vlx;

ObjectSystem::ObjectSystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id), m_objects(entity_admin, id)
{
	m_objects.Each([this](EntityID entity_id, Object& object)
		{
			if (!object.IsAlive)
				RemoveEntity(entity_id);
		});
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

void ObjectSystem::PreUpdate()
{
	ExecuteCommands(S_PreUpdate);
}

void ObjectSystem::Update()
{
	Execute();
	ExecuteCommands(S_Update);
}

void ObjectSystem::FixedUpdate()
{
	ExecuteCommands(S_FixedUpdate);
}

void ObjectSystem::PostUpdate()
{
	ExecuteCommands(S_PostUpdate);
}

void ObjectSystem::ExecuteCommands(ExecutionStage stage)
{
	auto& commands = m_command_table[stage];
	while (!commands.empty())
	{
		const auto& command = commands.front();
		VisitCommand(command);
		commands.pop();
	}
}

void ObjectSystem::VisitCommand(const Command& command)
{
	std::visit(traits::Overload
		{
			[this](AddCompData data)
			{
				m_entity_admin->AddComponent(data.entity_id, data.component_id);
			},
			[this](AddCompsData data)
			{
				m_entity_admin->AddComponents(data.entity_id, data.component_ids, data.archetype_id);
			},
			[this](RmvCompData data)
			{
				m_entity_admin->RemoveComponent(data.entity_id, data.component_id);
			},
			[this](RmvCompsData data)
			{
				m_entity_admin->RemoveComponents(data.entity_id, data.component_ids, data.archetype_id);
			},
			[this](RmvEntityData data)
			{
				m_entity_admin->RemoveEntity(data.entity_id);
			}
		}, command);
}
