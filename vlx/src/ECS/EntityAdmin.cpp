#include <Velox/ECS/EntityAdmin.h>
#include <Velox/ECS/Entity.h>

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
			const ComponentTypeID& component_id = archetype->type[i];
			const IComponentAlloc* component	= m_component_map[component_id].get();
			const std::size_t& component_size	= component->GetSize();

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
				component->DestroyData(*this, archetype->entities[j], &archetype->component_data[i][j * component_size]);
		}
	}
}

EntityID EntityAdmin::GetNewEntityID()
{
	if (!m_reusable_entity_ids.empty())
	{
		const EntityID entity_id = m_reusable_entity_ids.front();
		m_reusable_entity_ids.pop();

		return entity_id;
	}

	return m_entity_id_counter++;
}

const std::size_t& EntityAdmin::GetComponentIndex(const EntityID entity_id) const
{
	const auto it = m_entity_archetype_map.find(entity_id);
	if (it == m_entity_archetype_map.end())
		throw std::runtime_error("entity has not been registered");

	return it->second.index;
}

bool EntityAdmin::InArchetype(const ArchetypeID archetype, const EntityID entity_id) const
{
	const auto it = m_entity_archetype_map.find(entity_id);
	if (it == m_entity_archetype_map.end())
		return false;

	return it->second.archetype->id == archetype;
}

void EntityAdmin::RunSystems(const LayerType layer) const
{
	const auto it = m_systems.find(layer);
	if (it == m_systems.end())
		return;

	for (const ISystem* system : it->second)
	{
		const ArchetypeID& key = system->GetKey();

		auto it = m_archetype_map.find(key);
		if (it == m_archetype_map.end())
			continue;

		for (Archetype* archetype : it->second)
			system->DoAction(archetype);
	}
}

void EntityAdmin::SortSystems(const LayerType layer)
{
	auto& systems = m_systems[layer];
	std::stable_sort(systems.begin(), systems.end(),
		[](const ISystem* lhs, const ISystem* rhs)
		{
			return lhs->GetPriority() > rhs->GetPriority(); // in descending order
		});
}

void EntityAdmin::RegisterSystem(const LayerType layer, ISystem* system)
{
	m_systems[layer].push_back(system);
}

void EntityAdmin::RegisterEntity(const EntityID entity_id)
{
	auto insert = m_entity_archetype_map.insert(std::make_pair(entity_id, Record()));
	assert(insert.second);
}

void EntityAdmin::RemoveSystem(const LayerType layer, ISystem* system)
{
	if (!cu::Erase(m_systems[layer], system))
		throw std::runtime_error("attempted removal of non-existing system");
}

void EntityAdmin::RemoveEntity(const EntityID entity_id)
{
	auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return;

	Record& record		 = eit->second;
	Archetype* archetype = record.archetype;

	if (archetype == nullptr)
	{
		m_entity_archetype_map.erase(entity_id);
		m_reusable_entity_ids.push(entity_id);

		return;
	}

	EntityID last_entity_id = archetype->entities.back();
	Record& last_record		= m_entity_archetype_map[last_entity_id];

	const ComponentIDs& archetype_id = archetype->type;
	for (std::size_t i = 0; i < archetype_id.size(); ++i) // we iterate over both archetypes
	{
		const ComponentTypeID component_id	= archetype_id[i];
		const IComponentAlloc* component	= m_component_map[component_id].get();
		const std::size_t& component_size	= component->GetSize();

		component->DestroyData(*this, entity_id, &archetype->component_data[i][record.index * component_size]);

		if (last_entity_id != entity_id)
		{
			component->MoveDestroyData(*this, entity_id,
				&archetype->component_data[i][last_record.index * component_size],
				&archetype->component_data[i][record.index * component_size]);
		}
	}

	if (last_entity_id != entity_id)
	{
		archetype->entities[record.index] = archetype->entities.back(); // now swap ids with last
		last_record.index = record.index;
	}

	archetype->entities.pop_back();

	m_entity_archetype_map.erase(entity_id);
	m_reusable_entity_ids.push(entity_id);
}

Archetype* EntityAdmin::GetArchetype(const ComponentIDs& component_ids)
{
	const ArchetypeID id = cu::VectorHash<ComponentIDs>()(component_ids);

	const auto it = m_archetype_map.find(id);
	if (it != m_archetype_map.end())
	{
		Archetype* result = it->second.front();
		if (result->id == id)
			return result;
	}

	return CreateArchetype(component_ids, id); // archetype does not exist, create new one
}

Archetype* EntityAdmin::CreateArchetype(const ComponentIDs& component_ids, const ArchetypeID id)
{
	ArchetypePtr new_archetype = std::make_unique<Archetype>();

	new_archetype->id = id;
	new_archetype->type = component_ids;

	auto& archetypes = m_archetype_map[id];
	archetypes.insert(archetypes.begin(), new_archetype.get());

	for (auto width = 1; width <= std::ssize(component_ids) - 1; ++width)
	{
		for (std::size_t i = 0; (width + i) <= component_ids.size(); ++i)
		{
			const auto begin = (component_ids.begin() + i);
			const ComponentIDs subset_ids(begin, begin + width);
			const auto subset_id = cu::VectorHash<ComponentIDs>()(subset_ids);

			m_archetype_map[subset_id].push_back(new_archetype.get());
		}
	}

	new_archetype->component_data.reserve(component_ids.size()); // prevent any reallocations
	new_archetype->component_data_size.reserve(component_ids.size());

	for (ComponentIDs::size_type i = 0; i < component_ids.size(); ++i) // add empty array for each component in type
	{
		constexpr std::size_t DEFAULT_SIZE = 128; // default size in bytes to reduce number of reallocations

		new_archetype->component_data.push_back(std::make_unique<ByteArray>(DEFAULT_SIZE));
		new_archetype->component_data_size.push_back(DEFAULT_SIZE);

		m_component_archetypes_map[component_ids[i]][new_archetype->id].column = i;
	}

#ifdef VELOX_DEBUG
	for (const auto& archetype : m_archetypes)
	{
		for (const auto& archetype1 : m_archetypes)
		{
			if (archetype.get() == archetype1.get())
				continue;

			assert(archetype->type != archetype1->type); // no duplicates
		}
	}
#endif

	return m_archetypes.emplace_back(std::move(new_archetype)).get();
}

void EntityAdmin::Shrink(bool extensive)
{
	// remove any dangling archetypes with no entities
	m_archetypes.erase(std::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype->entities.empty())
			{
				cu::Erase(m_archetype_map[archetype->id], archetype.get());

				for (auto i = std::ssize(archetype->type) - 2; i >= 0; --i)
				{
					const ComponentIDs subset_ids(archetype->type.begin(), archetype->type.begin() + (i + 1));
					const auto subset_id = cu::VectorHash<ComponentIDs>()(subset_ids);

					cu::Erase(m_archetype_map[subset_id], archetype.get());
				}

				return true;
			}

			return false;
		}), m_archetypes.end());

	// remove any archetypes that holds no components (cant be used anyways), should not exist in the first place
	m_archetypes.erase(std::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype->type.empty())
			{
				cu::Erase(m_archetype_map[archetype->id], archetype.get());

				for (auto i = std::ssize(archetype->type) - 2; i >= 0; --i)
				{
					const ComponentIDs subset_ids(archetype->type.begin(), archetype->type.begin() + (i + 1));
					const auto subset_id = cu::VectorHash<ComponentIDs>()(subset_ids);

					cu::Erase(m_archetype_map[subset_id], archetype.get());
				}

				for (const EntityID entity_id : archetype->entities)
				{
					m_entity_archetype_map.erase(entity_id);
					m_reusable_entity_ids.push(entity_id);
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
			const ComponentIDs& archetype_id = archetype->type;

			for (std::size_t j = 0; j < archetype_id.size(); ++j)
			{
				const ComponentTypeID& component_id	= archetype->type[i];
				const IComponentAlloc* component	= m_component_map[component_id].get();
				const std::size_t& component_size	= component->GetSize();

				const std::size_t current_size		= archetype->entities.size() * component_size;

				if (archetype->component_data_size[j] > current_size)
				{
					archetype->component_data_size[j] = current_size;
					ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[j]);

					for (std::size_t k = 0; k < archetype->entities.size(); ++k)
					{
						component->MoveDestroyData(*this, archetype->entities[k],
							&archetype->component_data[j][k * component_size],
							&new_data[k * component_size]);
					}

					archetype->component_data[j] = std::move(new_data);
				}
			}
		}
	}
}

void EntityAdmin::AddComponent(const EntityID entity_id, const ComponentTypeID add_component_id) // TODO: fix ugly duplicate code that may cause future issues
{
	assert(m_component_map.contains(add_component_id)); // component should be registered

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return;

	Record& record = eit->second;
	Archetype* old_archetype = record.archetype;

	Archetype* new_archetype = nullptr;

	if (old_archetype) // already has an attached archetype, define a new archetype
	{
		ComponentIDs new_archetype_id = old_archetype->type; // create copy
		if (!cu::InsertUniqueSorted(new_archetype_id, add_component_id)) // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)
			return;

		new_archetype = GetArchetype(new_archetype_id);

		const EntityID last_entity_id = old_archetype->entities.back();
		Record& last_record = m_entity_archetype_map[last_entity_id];

		const bool same_entity = (last_entity_id == entity_id);

		assert(new_archetype_id == new_archetype->type);

		for (std::size_t i = 0, j = 0; i < new_archetype_id.size(); ++i) // move all the data from old to new and perform swaps at the same time
		{
			const ComponentTypeID component_id	= new_archetype_id[i];
			const IComponentAlloc* component	= m_component_map[component_id].get();
			const std::size_t& component_size	= component->GetSize();

			const std::size_t current_size		= new_archetype->entities.size() * component_size;
			const std::size_t new_size			= current_size + component_size;

			if (new_size > new_archetype->component_data_size[i])
				MakeRoom(new_archetype, component, component_size, i);

			if (component_id == add_component_id)
			{
				component->ConstructData(*this, entity_id, &new_archetype->component_data[i][current_size]);
			}
			else
			{
				component->MoveDestroyData(*this, entity_id,
					&old_archetype->component_data[j][record.index * component_size],
					&new_archetype->component_data[i][current_size]);

				if (!same_entity)
				{
					component->MoveDestroyData(*this, entity_id,
						&old_archetype->component_data[j][last_record.index * component_size],
						&old_archetype->component_data[j][record.index * component_size]); // move data to last
				}

				++j;
			}
		}

		if (!same_entity) // move back to current
		{
			old_archetype->entities[record.index] = old_archetype->entities.back();
			last_record.index = record.index;
		}

		old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next component is added, it will overwrite the previous
	}
	else // if the entity has no archetype, first component
	{
		ComponentIDs new_archetype_id(1, add_component_id);	// construct archetype with the component id
		new_archetype = GetArchetype(new_archetype_id);		// construct or get archetype using the id

		const IComponentAlloc* component	= m_component_map[add_component_id].get();
		const std::size_t& component_size	= component->GetSize();

		const std::size_t current_size		= new_archetype->entities.size() * component_size;
		const std::size_t new_size			= current_size + component_size;

		if (new_size > new_archetype->component_data_size[0])
			MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

		component->ConstructData(*this, entity_id, &new_archetype->component_data[0][current_size]);
	}

	new_archetype->entities.push_back(entity_id);
	record.index = new_archetype->entities.size() - 1;
	record.archetype = new_archetype;
}

void EntityAdmin::RemoveComponent(const EntityID entity_id, const ComponentTypeID rmv_component_id)
{
	assert(m_component_map.contains(rmv_component_id));

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return;

	Record& record = eit->second;
	Archetype* old_archetype = record.archetype;

	if (!old_archetype)
		return;

	ComponentIDs new_archetype_id = old_archetype->type;
	if (!cu::Erase(new_archetype_id, rmv_component_id)) // component did not exist
		return;

	Archetype* new_archetype = GetArchetype(new_archetype_id);

	const EntityID last_entity_id = old_archetype->entities.back();
	Record& last_record = m_entity_archetype_map[last_entity_id];

	const bool same_entity = (last_entity_id == entity_id);

	const ComponentIDs& old_archetype_id = old_archetype->type;
	for (std::size_t i = 0, j = 0; i < old_archetype_id.size(); ++i) // we iterate over both archetypes
	{
		const ComponentTypeID component_id	= old_archetype_id[i];
		const IComponentAlloc* component	= m_component_map[component_id].get();
		const std::size_t& component_size	= component->GetSize();

		if (component_id == rmv_component_id)
		{
			component->DestroyData(*this, entity_id, &old_archetype->component_data[i][record.index * component_size]);
		}
		else
		{
			const std::size_t current_size	= new_archetype->entities.size() * component_size;
			const std::size_t new_size		= current_size + component_size;

			if (new_size > new_archetype->component_data_size[j])
				MakeRoom(new_archetype, component, component_size, j); // make room to fit data

			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[i][record.index * component_size],
				&new_archetype->component_data[j][current_size]); // move all the valid data from old to new

			++j;
		}

		if (!same_entity) // no point of swapping data with itself
		{
			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[i][last_record.index * component_size],
				&old_archetype->component_data[i][record.index * component_size]); // move data to last
		}
	}

	if (!same_entity)
	{
		old_archetype->entities[record.index] = old_archetype->entities.back(); // now swap ids
		last_record.index = record.index;
	}

	old_archetype->entities.pop_back();
	new_archetype->entities.push_back(entity_id);

	record.index = new_archetype->entities.size() - 1;
	record.archetype = new_archetype;
}

void EntityAdmin::MakeRoom(
	Archetype* archetype, 
	const IComponentAlloc* component, 
	const std::size_t data_size,
	const std::size_t i)
{
	archetype->component_data_size[i] *= 2;
	archetype->component_data_size[i] += data_size;

	ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[i]);

	for (std::size_t j = 0; j < archetype->entities.size(); ++j)
	{
		component->MoveDestroyData(*this, archetype->entities[j],
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