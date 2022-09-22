#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

EntityAdmin::EntityAdmin() : m_entity_id_counter(1) 
{

}

EntityAdmin::~EntityAdmin()
{
	for (auto& archetype : m_archetypes)
	{
		for (std::size_t i = 0; i < archetype->type.size(); ++i)
		{
			const ComponentBase* comp = m_component_map[archetype->type[i]].get();
			const std::size_t& data_size = comp->GetSize();

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
				comp->DestroyData(&archetype->component_data[i][j * data_size]);
		}
	}
}

EntityID EntityAdmin::GetNewId()
{
	return m_entity_id_counter++;
}

void EntityAdmin::RunSystems(const std::uint8_t layer, Time& time)
{
	for (SystemBase* system : m_systems[layer])
	{
		const ArchetypeID& key = system->GetKey();
		for (ArchetypePtr& archetype : m_archetypes)
		{
			if (std::includes(archetype->type.begin(), archetype->type.end(), key.begin(), key.end()))
			{
				system->DoAction(time, archetype.get());
			}
		}
	}
}
void EntityAdmin::SortSystems(const std::uint8_t layer)
{
	auto& systems = m_systems[layer];
	std::sort(systems.begin(), systems.end(),
		[](const SystemBase* lhs, const SystemBase* rhs)
		{
			return lhs->GetPriority() > rhs->GetPriority(); // in descending order
		});
}

void EntityAdmin::RegisterSystem(const std::uint8_t layer, SystemBase* system)
{
	m_systems[layer].push_back(system);
}
void EntityAdmin::RegisterEntity(const EntityID entity_id)
{
	auto insert = m_entity_archetype_map.insert(std::make_pair(entity_id, Record()));
	assert(insert.second);
}

void EntityAdmin::RemoveSystem(const std::uint16_t layer, SystemBase* system)
{
	if (!cu::Erase(m_systems[layer], system))
		throw std::runtime_error("attempted removal of non-existing system");
}
void EntityAdmin::RemoveEntity(const EntityID entity_id)
{
	if (!m_entity_archetype_map.contains(entity_id))
		return;

	Record& record = m_entity_archetype_map[entity_id];
	Archetype* old_archetype = record.archetype;

	if (!old_archetype)
	{
		m_entity_archetype_map.erase(entity_id);
		return;
	}

	for (std::size_t i = 0; i < old_archetype->type.size(); ++i)
	{
		const ComponentTypeID& component_id = old_archetype->type[i];
		const ComponentBase* component = m_component_map[component_id].get();
		const std::size_t& component_size = component->GetSize();

		component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);
	}

	for (std::size_t i = 0; i < old_archetype->type.size(); ++i)
	{
		const ComponentTypeID& component_id = old_archetype->type[i];
		const ComponentBase* component = m_component_map[component_id].get();
		const std::size_t& component_size = component->GetSize();

		EraseComponent(old_archetype, component, record.index, i);
	}

	m_entity_archetype_map.erase(entity_id);

	auto it = std::find(old_archetype->entities.begin(), old_archetype->entities.end(), entity_id);

	std::for_each(it, old_archetype->entities.end(),
		[this, &entity_id](const EntityID& eid)
		{
			if (eid == entity_id)
				return;

			--m_entity_archetype_map[eid].index;
		});

	old_archetype->entities.erase(it);
}

Archetype* EntityAdmin::GetArchetype(const ArchetypeID& id)
{
	for (const ArchetypePtr& archetype : m_archetypes)
	{
		if (archetype->type == id)
			return archetype.get();
	}

	// archetype does not exist, create new one

	ArchetypePtr new_archetype = ArchetypePtr(new Archetype);
	new_archetype->type = id;

	for (ArchetypeID::size_type i = 0; i < id.size(); ++i) // add empty array for each component in type
	{
		new_archetype->component_data.push_back(std::make_unique<ByteArray>(0));
		new_archetype->component_data_size.push_back(0);
	}

	return m_archetypes.emplace_back(std::move(new_archetype)).get();
}

void EntityAdmin::MakeRoom(
	Archetype* archetype, 
	const ComponentBase* component, 
	const size_t data_size,
	const size_t i)
{
	archetype->component_data_size[i] *= 2;
	archetype->component_data_size[i] += data_size;

	ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[i]);

	for (std::size_t j = 0; j < archetype->entities.size(); ++j)
	{
		component->MoveDestroyData(
			&archetype->component_data[i][j * data_size], 
			&new_data[j * data_size]);
	}

	archetype->component_data[i] = std::move(new_data);
}

bool EntityAdmin::MoveComponent(
	Archetype* old_archetype,
	Archetype* new_archetype,
	const ComponentBase* component,
	const std::size_t component_id,
	const std::size_t old_component,
	const std::size_t new_component,
	const std::size_t i)
{
	const ArchetypeID& old_archetype_id = old_archetype->type;
	for (std::size_t j = 0; j < old_archetype_id.size(); ++j)
	{
		const ComponentTypeID& old_component_id = old_archetype_id[j];
		if (old_component_id == component_id)
		{
			component->MoveDestroyData(
				&old_archetype->component_data[j][old_component],
				&new_archetype->component_data[i][new_component]);

			return true;
		}
	}

	return false;
}

void EntityAdmin::EraseComponent(
	Archetype* archetype, 
	const ComponentBase* component,
	const std::size_t record_index, 
	const std::size_t i)
{
	const std::size_t component_size = component->GetSize();

	ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[i] - component_size);
	archetype->component_data_size[i] -= component_size;

	for (std::size_t j = 0, ri = 0; j < archetype->entities.size(); ++j)
	{
		if (j == record_index)
			continue;

		component->MoveDestroyData(
			&archetype->component_data[i][j * component_size],
			&new_data[ri * component_size]);

		++ri;
	}

	archetype->component_data[i] = std::move(new_data);
}