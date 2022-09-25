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
	if (!m_reusable_ids.empty())
	{
		EntityID entity_id = m_reusable_ids.front();
		m_reusable_ids.pop();

		return entity_id;
	}

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
	auto eit = m_entity_archetype_map.find(entity_id);

	if (eit == m_entity_archetype_map.end())
		return;

	Record& record = eit->second;
	Archetype* archetype = record.archetype;

	if (!archetype)
	{
		m_entity_archetype_map.erase(entity_id);
		m_reusable_ids.push(entity_id);

		return;
	}

	ArchetypeID& archetype_id = archetype->type;

	EntityID last_entity_id = archetype->entities.back();
	Record& last_record = m_entity_archetype_map[last_entity_id];

	if (last_entity_id != entity_id)
	{
		for (std::size_t i = 0; i < archetype_id.size(); ++i)
		{
			const ComponentTypeID& component_id = archetype->type[i];
			const ComponentBase* component = m_component_map[component_id].get();
			const std::size_t& component_size = component->GetSize();

			component->DestroyData(&archetype->component_data[i][record.index * component_size]);

			component->MoveDestroyData(
				&archetype->component_data[i][last_record.index * component_size],
				&archetype->component_data[i][record.index * component_size]);
		}

		archetype->entities.at(record.index) = archetype->entities.back(); // now swap ids (using *.at() because the flow is slightly confusing)
		last_record.index = record.index;
	}

	archetype->entities.pop_back();

	m_entity_archetype_map.erase(entity_id);
	m_reusable_ids.push(entity_id);
}

Archetype* EntityAdmin::GetArchetype(const ArchetypeID& id)
{
	auto it = m_archetype_map.find(id);

	if (it != m_archetype_map.end())
		return it->second;

	// archetype does not exist, create new one

	ArchetypePtr new_archetype = ArchetypePtr(new Archetype);

	new_archetype->id = cu::VectorHash<ArchetypeID>()(id);
	new_archetype->type = id;

	auto insert = m_archetype_map.insert(std::make_pair(new_archetype->type, new_archetype.get()));
	assert(insert.second);

	for (ArchetypeID::size_type i = 0; i < id.size(); ++i) // add empty array for each component in type
	{
		constexpr std::size_t DEFAULT_SIZE = 64; // default size in bytes to reduce number of reallocations

		new_archetype->component_data.push_back(std::make_unique<ByteArray>(DEFAULT_SIZE));
		new_archetype->component_data_size.push_back(DEFAULT_SIZE);
	}

	return m_archetypes.emplace_back(std::move(new_archetype)).get();
}

void EntityAdmin::Shrink(bool extensive)
{
	// remove any dangling archetypes with no entities
	m_archetypes.erase(std::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[](const ArchetypePtr& archetype)
		{
			return archetype->entities.empty();
		}), m_archetypes.end());

	// remove any archetypes that holds no components (cant be used anyways)
	m_archetypes.erase(std::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype->type.empty())
			{
				for (const EntityID entity_id : archetype->entities)
				{
					m_entity_archetype_map.erase(entity_id);
					m_reusable_ids.push(entity_id);
				}
				return true;
			}
			return false;
		}), m_archetypes.end());

	if (extensive) // shrink all archetypes data
	{
		for (std::size_t i = 0; i < m_archetypes.size(); ++i)
		{
			const ArchetypePtr& archetype = m_archetypes[i];
			const ArchetypeID& archetype_id = archetype->type;
			const std::vector<EntityID>& entities = archetype->entities;

			for (std::size_t j = 0; j < archetype_id.size(); ++j)
			{
				const ComponentTypeID& component_id = archetype->type[i];
				const ComponentBase* component = m_component_map[component_id].get();
				const std::size_t& component_size = component->GetSize();

				const std::size_t current_size = entities.size() * component_size;

				archetype->component_data_size[j] = current_size;
				ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[j]);

				for (std::size_t k = 0; k < entities.size(); ++k)
				{
					component->MoveDestroyData(
						&archetype->component_data[j][k * component_size],
						&new_data[k * component_size]);
				}

				archetype->component_data[j] = std::move(new_data);
			}
		}
	}
}

void EntityAdmin::Sort()
{

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

// -- old remove entity --
// 
//Record& record = it->second;
//Archetype* old_archetype = record.archetype;

//if (!old_archetype)
//{
//	m_entity_archetype_map.erase(it);
//	return;
//}

//for (std::size_t i = 0; i < old_archetype->type.size(); ++i)
//{
//	const ComponentTypeID& component_id = old_archetype->type[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);
//}

//for (std::size_t i = 0; i < old_archetype->type.size(); ++i)
//{
//	const ComponentTypeID& component_id = old_archetype->type[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	EraseComponent(old_archetype, component, record.index, i);
//}

//m_entity_archetype_map.erase(entity_id);

//auto it = std::find(old_archetype->entities.begin(), old_archetype->entities.end(), entity_id);

//std::for_each(it, old_archetype->entities.end(),
//	[this, &entity_id](const EntityID& eid)
//	{
//		if (eid == entity_id)
//			return;

//		--m_entity_archetype_map[eid].index;
//	});

//old_archetype->entities.erase(it);