#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

EntityAdmin::EntityAdmin() : m_entity_id_counter(1) 
{

}

EntityAdmin::~EntityAdmin()
{
	for (auto& archetype : m_archetypes)
	{
		for (std::size_t i = 0; i < archetype->m_type.size(); ++i)
		{
			const ComponentBase* const comp = m_component_map[archetype->m_type[i]].get();
			const std::size_t& data_size = comp->GetSize();

			for (std::size_t j = 0; j < archetype->m_entity_ids.size(); ++j)
			{
				comp->DestroyData(&archetype->m_component_data[i][j * data_size]);
			}
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
		for (const ArchetypePtr& archetype : m_archetypes)
		{
			if (std::includes(archetype->m_type.begin(), archetype->m_type.end(), key.begin(), key.end()))
			{
				system->Update(time, archetype.get());
			}
		}
	}
}

void EntityAdmin::RegisterSystem(const std::uint8_t layer, SystemBase* system)
{
	m_systems[layer].push_back(system);
}
void EntityAdmin::RegisterEntity(const EntityID entity_id)
{
	m_entity_archetype_map[entity_id] = Record();
}

void EntityAdmin::RemoveEntity(const EntityID entity_id)
{

}

Archetype* EntityAdmin::GetArchetype(const ArchetypeID& id)
{
	for (const ArchetypePtr& archetype : m_archetypes)
	{
		if (archetype->m_type == id)
			return archetype.get();
	}

	// archetype does not exist, create new one

	ArchetypePtr new_archetype = ArchetypePtr(new Archetype());
	new_archetype->m_type = id;

	for (ArchetypeID::size_type i = 0; i < id.size(); ++i) // add empty array for each component in type
	{
		new_archetype->m_component_data.push_back(ComponentData(new unsigned char[0])); // C++17 supports array delete
		new_archetype->m_component_data_size.push_back(0);
	}

	return m_archetypes.emplace_back(std::move(new_archetype)).get();
}

void EntityAdmin::MakeRoom(Archetype* new_archetype, const ComponentBase* new_comp, const size_t data_size, const size_t i)
{
	new_archetype->m_component_data_size[i] *= 2;
	new_archetype->m_component_data_size[i] += data_size;

	ComponentData new_data = ComponentData(new unsigned char[new_archetype->m_component_data_size[i]]);

	for (std::size_t j = 0; j < new_archetype->m_entity_ids.size(); ++j)
	{
		new_comp->MoveDestroyData(
			&new_archetype->m_component_data[i][j * data_size], &new_data[j * data_size]);
	}

	new_archetype->m_component_data[i] = std::move(new_data);
}