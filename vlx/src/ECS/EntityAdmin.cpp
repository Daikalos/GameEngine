#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

EntityAdmin::~EntityAdmin()
{
	Destroy();
}

bool EntityAdmin::IsComponentRegistered(ComponentTypeID component_id) const
{
	return m_component_map.contains(component_id);
}

bool EntityAdmin::IsComponentsRegistered(ComponentIDSpan component_ids) const
{
	if (component_ids.empty())
		return false;

	for (ComponentTypeID id : component_ids)
	{
		if (!IsComponentRegistered(id))
			return false;
	}

	return true;
}

std::vector<EntityID> EntityAdmin::GetEntitiesWith(ComponentIDSpan component_ids, ArchetypeID archetype_id, bool restricted) const
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids));

	std::vector<EntityID> entities;

	if (!restricted)
	{
		for (const Archetype* archetype : GetArchetypes(component_ids, archetype_id))
		{
			entities.insert(entities.end(),
				archetype->entities.begin(),
				archetype->entities.end());
		}
	}
	else
	{
		const auto it = m_archetype_map.find(archetype_id);
		if (it == m_archetype_map.end())
			return entities;

		entities.insert(entities.end(),
			it->second->entities.begin(),
			it->second->entities.end());
	}

	return entities;
}

void EntityAdmin::Reserve(ComponentIDSpan component_ids, ArchetypeID archetype_id, std::size_t component_count)
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids));

	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	Archetype* archetype = GetArchetype(component_ids, archetype_id);
	for (std::size_t i = 0; i < archetype->type.size(); ++i)
	{
		const auto component		= m_component_map[archetype->type[i]].get();
		const auto component_size	= component->GetSize();

		const auto current_size		= archetype->component_data_size[i];
		const auto new_size			= component_count * component_size;

		if (new_size > current_size) // only reserve if larger than current size
		{
			auto new_data = std::make_unique_for_overwrite<ByteArray>(new_size);

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
			{
				component->MoveDestroyData(*this, archetype->entities[j],
					&archetype->component_data[i][j * component_size],
					&new_data[j * component_size]);
			}

			archetype->component_data_size[i] = new_size;
			archetype->component_data[i] = std::move(new_data);
		}
	}
}

EntityID EntityAdmin::GetNewEntityID()
{
	const auto GenerateID = [this]()
	{
		if (!m_reusable_entity_ids.empty())
		{
			EntityID entity_id = m_reusable_entity_ids.front();
			m_reusable_entity_ids.pop();

			return entity_id;
		}

		return m_entity_id_counter++;
	};

	const EntityID entity_id = GenerateID();

	m_generation_count_map[entity_id]++; // increment generation count

	return entity_id;
}

std::size_t EntityAdmin::GetGenerationCount(EntityID entity_id) const
{
	return m_generation_count_map.at(entity_id);
}

bool EntityAdmin::IsEntityRegistered(EntityID entity_id) const
{
	return m_entity_archetype_map.contains(entity_id);
}
bool EntityAdmin::HasComponent(EntityID entity_id, ComponentTypeID component_id) const
{
	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return false;

	const Archetype* archetype = eit->second.archetype;

	if (archetype == nullptr)
		return false;

	const auto cit = m_component_archetypes_map.find(component_id);
	if (cit == m_component_archetypes_map.end())
		return false;

	return cit->second.contains(archetype->id);
}

auto EntityAdmin::RegisterEntity(EntityID entity_id) -> Record&
{
	assert(entity_id != NULL_ENTITY && "Cannot register a null entity id");

	auto [it, success] = m_entity_archetype_map.try_emplace(entity_id, Record());
	assert(success);

	return it->second;
}
bool EntityAdmin::RegisterSystem(LayerType layer, SystemBase* system)
{
	assert(layer != LYR_NONE && "System cannot be added to null layer");

	if (m_system_lock)
		throw std::runtime_error("Systems are currently locked from modifications");

	if (system == nullptr)
		return false;

	auto& systems = m_systems[layer];

	const auto it = std::find_if(systems.rbegin(), systems.rend(),
		[&system](const SystemBase* sys)
		{
			return system == sys;
		});

	if (it != systems.rend()) // already exists
		return false;

	systems.emplace_back(system);

	return true;
}

void EntityAdmin::RunSystems(LayerType layer) const
{
	const auto sit = m_systems.find(layer);
	if (sit == m_systems.end())
		return;

	m_system_lock = true;

	for (const SystemBase* system : sit->second)
		RunSystem(system);

	m_system_lock = false;
}

void EntityAdmin::SortSystems(LayerType layer)
{
	const auto it = m_systems.find(layer);
	if (it == m_systems.end())
		return;

	if (m_system_lock)
		throw std::runtime_error("Systems are currently locked from modifications");

	std::ranges::stable_sort(it->second.begin(), it->second.end(),
		[](const SystemBase* lhs, const SystemBase* rhs)
		{
			return *lhs > *rhs;
		});
}

void EntityAdmin::RunSystem(const SystemBase* system) const
{
	if (!system->IsEnabled())
		return;

	system->Start();

	const auto& archetypes = GetArchetypes(system->GetArchKey(), system->GetIDKey());

	m_component_lock = true;

	if (system->IsRunningParallel())
	{
		std::for_each(std::execution::par, archetypes.begin(), archetypes.end(),
			[&system](const Archetype* const archetype)
			{
				system->Run(archetype);
			});
	}
	else
	{
		std::ranges::for_each(archetypes.begin(), archetypes.end(),
			[&system](const Archetype* const archetype)
			{
				system->Run(archetype);
			});
	}

	m_component_lock = false;

	system->End();
}

bool EntityAdmin::RemoveSystem(LayerType layer, SystemBase* system)
{
	if (m_system_lock)
		throw std::runtime_error("Systems are currently locked from modifications");

	return cu::Erase(m_systems[layer], system);
}

bool EntityAdmin::RemoveEntity(EntityID entity_id)
{
	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end()) // entity does not exist
		return false;

	Record& record		 = eit->second;
	Archetype* archetype = record.archetype;

	if (archetype == nullptr) // entity has not been assigned an archetype anyways
	{
		m_entity_archetype_map.erase(entity_id);
		m_reusable_entity_ids.push(entity_id);

		return true;
	}

	EntityID last_entity_id = archetype->entities.back();

	if (last_entity_id != entity_id)
	{
		Record& last_record = m_entity_archetype_map[last_entity_id];

		for (std::size_t i = 0; i < archetype->type.size(); ++i)
		{
			const auto component_id		= archetype->type[i];
			const auto component		= m_component_map[component_id].get();
			const auto component_size	= component->GetSize();

			const auto entity_data		= &archetype->component_data[i][record.index * component_size];
			const auto last_entity_data	= &archetype->component_data[i][last_record.index * component_size];

			component->DestroyData(*this, entity_id, entity_data);
			component->MoveDestroyData(*this, last_entity_id, last_entity_data, entity_data); // move data from current to last
		}

		archetype->entities[record.index] = last_entity_id; // now swap ids with last
		last_record.index = record.index;
	}
	else
	{
		for (std::size_t i = 0; i < archetype->type.size(); ++i)
		{
			const auto component_id		= archetype->type[i];
			const auto component		= m_component_map[component_id].get();
			const auto component_size	= component->GetSize();

			component->DestroyData(*this, entity_id, &archetype->component_data[i][record.index * component_size]);
		}
	}

	archetype->entities.pop_back();

	m_entity_archetype_map.erase(entity_id);
	m_reusable_entity_ids.push(entity_id);

	return true;
}

void EntityAdmin::AddComponent(EntityID entity_id, ComponentTypeID add_component_id)
{
	static const auto component_ids	= { add_component_id };
	static const auto archetype_id	= cu::ContainerHash<ComponentTypeID>()(component_ids);

	AddComponents(entity_id, component_ids, archetype_id);
}

bool EntityAdmin::RemoveComponent(EntityID entity_id, ComponentTypeID rmv_component_id)
{
	static const auto component_ids	= { rmv_component_id };
	static const auto archetype_id	= cu::ContainerHash<ComponentTypeID>()(component_ids);

	return RemoveComponents(entity_id, component_ids, archetype_id);
}

void EntityAdmin::AddComponents(EntityID entity_id, ComponentIDSpan component_ids, ArchetypeID archetype_id)
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids));
	assert(!component_ids.empty());

	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return;

	Record& record = eit->second;
	Archetype* old_archetype = record.archetype;

	Archetype* new_archetype = nullptr; // we are going to be moving to a new archetype

	if (old_archetype) // already has an attached archetype, define a new archetype
	{
		if (const auto ait = old_archetype->edges.find(archetype_id); ait == old_archetype->edges.end())
		{
			ComponentIDs new_archetype_id = old_archetype->type; // create copy

			bool found = false;
			for (ComponentTypeID component_id : component_ids) // determine valid components
			{
				if (cu::InsertUniqueSorted<ComponentTypeID>(new_archetype_id, component_id)) // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)
					found = true;
			}

			if (!found) // unable to add any component
				return;

			new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentTypeID>()(new_archetype_id));

			old_archetype->edges[archetype_id].add = new_archetype;
			new_archetype->edges[archetype_id].rmv = old_archetype;

			assert(new_archetype_id != old_archetype->type);
			assert(new_archetype_id == new_archetype->type);
		}
		else new_archetype = ait->second.add;

		if (!new_archetype) // exit if no archetype was found
			return;

		EntityID last_entity_id = old_archetype->entities.back();

		if (last_entity_id != entity_id)
		{
			Record& last_record = m_entity_archetype_map[last_entity_id];
			ConstructSwap(new_archetype, old_archetype, entity_id, record, last_entity_id, last_record);
		}
		else
		{
			Construct(new_archetype, old_archetype, entity_id, record);
		}

		old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next component is added, it will overwrite the previous
	}
	else // if the entity has no archetype, first components
	{
		new_archetype = GetArchetype(component_ids, archetype_id); // construct or get archetype using the id

		for (std::size_t i = 0; i < component_ids.size(); ++i)
		{
			const auto component_id		= component_ids[i];
			const auto component		= m_component_map[component_id].get();
			const auto component_size	= component->GetSize();

			const auto current_size = new_archetype->entities.size() * component_size;
			const auto new_size		= current_size + component_size;

			if (new_size > new_archetype->component_data_size[i])
				MakeRoom(new_archetype, component, component_size, i); // make room and move over existing data

			component->ConstructData(*this, entity_id, &new_archetype->component_data[i][current_size]);
		}
	}

	new_archetype->entities.push_back(entity_id);
	record.index = IDType(new_archetype->entities.size() - 1);
	record.archetype = new_archetype;
}

bool EntityAdmin::RemoveComponents(EntityID entity_id, ComponentIDSpan component_ids, ArchetypeID archetype_id)
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids) && !component_ids.empty());

	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return false;

	Record& record = eit->second;
	Archetype* old_archetype = record.archetype;

	if (old_archetype == nullptr) // not registered anyways, nothing to remove from
		return false;

	Archetype* new_archetype = nullptr;

	if (auto ait = old_archetype->edges.find(archetype_id); ait == old_archetype->edges.end())
	{
		ComponentIDs new_archetype_id = old_archetype->type; // create copy

		bool found = false;
		for (ComponentTypeID component_id : component_ids) // erase while keeping the vector sorted
		{
			if (cu::EraseUniqueSorted<ComponentTypeID>(new_archetype_id, component_id))
				found = true;
		}

		if (!found) // unable to remove any component
			return false;

		new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentTypeID>()(new_archetype_id));

		new_archetype->edges[archetype_id].add = old_archetype;
		old_archetype->edges[archetype_id].rmv = new_archetype;
	}
	else new_archetype = ait->second.rmv;

	if (!new_archetype) // exit if no archetype was found
		return false;

	EntityID last_entity_id = old_archetype->entities.back();

	if (last_entity_id != entity_id)
	{
		Record& last_record = m_entity_archetype_map[last_entity_id];
		DestructSwap(old_archetype, new_archetype, entity_id, record, last_entity_id, last_record);
	}
	else
	{
		Destruct(old_archetype, new_archetype, entity_id, record);
	}

	old_archetype->entities.pop_back();
	new_archetype->entities.push_back(entity_id);

	record.index = IDType(new_archetype->entities.size() - 1);
	record.archetype = new_archetype;

	return true;
}

void EntityAdmin::DeregisterOnAddListener(ComponentTypeID component_id, typename EventHandler<>::IDType id)
{
	if (auto it = m_events_add.find(component_id); it != m_events_add.end())
		it->second -= id;
}

void EntityAdmin::DeregisterOnMoveListener(ComponentTypeID component_id, typename EventHandler<>::IDType id)
{
	if (auto it = m_events_move.find(component_id); it != m_events_move.end())
		it->second -= id;
}

void EntityAdmin::DeregisterOnRemoveListener(ComponentTypeID component_id, typename EventHandler<>::IDType id)
{
	if (auto it = m_events_remove.find(component_id); it != m_events_remove.end())
		it->second -= id;
}

bool EntityAdmin::HasShutdown() const
{
	return m_shutdown;
}

void EntityAdmin::Shutdown()
{
	m_shutdown = true;
	Destroy(); // invalidate the ecs and destroy all data
}

Archetype* EntityAdmin::GetArchetype(ComponentIDSpan component_ids, ArchetypeID archetype_id)
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids));

	const auto it = m_archetype_map.find(archetype_id);
	if (it != m_archetype_map.end())
		return it->second;

	return CreateArchetype(component_ids, archetype_id); // archetype does not exist, create new one
}

const std::vector<Archetype*>& EntityAdmin::GetArchetypes(ComponentIDSpan component_ids, ArchetypeID archetype_id) const
{
	const auto it = m_archetype_cache.find(archetype_id);
	if (it != m_archetype_cache.end())
		return it->second;

	std::vector<Archetype*> result;

	for (const ArchetypePtr& archetype : m_archetypes)
	{
		if (std::ranges::includes(archetype->type.begin(), archetype->type.end(), component_ids.begin(), component_ids.end()))
			result.push_back(archetype.get());
	}

	return m_archetype_cache.emplace(archetype_id, result).first->second;
}

Archetype* EntityAdmin::CreateArchetype(ComponentIDSpan component_ids, ArchetypeID archetype_id)
{
	assert(cu::IsSorted<ComponentTypeID>(component_ids));

	auto new_archetype = std::make_unique<Archetype>();

	new_archetype->id = archetype_id;
	new_archetype->type = { component_ids.begin(), component_ids.end() };

	m_archetype_map[archetype_id] = new_archetype.get();

	for (uint64 i = 0; i < new_archetype->type.size(); ++i) // add empty array for each component in type
	{
		constexpr uint64 DEFAULT_BYTE_SIZE = 128; // default size in bytes to reduce number of reallocations

		new_archetype->component_data.emplace_back(std::make_unique_for_overwrite<ByteArray>(DEFAULT_BYTE_SIZE));
		new_archetype->component_data_size.emplace_back(DEFAULT_BYTE_SIZE);

		m_component_archetypes_map[new_archetype->type[i]][archetype_id].column = ColumnType(i);
	}

	m_archetype_cache.clear(); // unfortunately for now, we'll have to clear the cache whenever an archetype has been added

	return m_archetypes.emplace_back(std::move(new_archetype)).get();
}

void EntityAdmin::CallOnAddEvent(ComponentTypeID component_id, EntityID eid, void* data) const
{
	if (auto it = m_events_add.find(component_id); it != m_events_add.end())
		it->second(eid, data);
}

void EntityAdmin::CallOnMoveEvent(ComponentTypeID component_id, EntityID eid, void* data) const
{
	if (auto it = m_events_move.find(component_id); it != m_events_move.end())
		it->second(eid, data);
}

void EntityAdmin::CallOnRemoveEvent(ComponentTypeID component_id, EntityID eid, void* data) const
{
	if (auto it = m_events_remove.find(component_id); it != m_events_remove.end())
		it->second(eid, data);
}

EntityID EntityAdmin::Duplicate(EntityID entity_id)
{
	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return NULL_ENTITY;

	Record& record = eit->second;
	Archetype* archetype = record.archetype;

	if (archetype == nullptr)
		return NULL_ENTITY;

	EntityID new_entity_id = GetNewEntityID();
	Record& new_record = RegisterEntity(new_entity_id);

	for (std::size_t i = 0; i < archetype->type.size(); ++i)
	{
		const auto component_id		= archetype->type[i];
		const auto component		= m_component_map[component_id].get();
		const auto component_size	= component->GetSize();

		const auto current_size = archetype->entities.size() * component_size;
		const auto new_size		= current_size + component_size;

		if (new_size > archetype->component_data_size[i])
			MakeRoom(archetype, component, component_size, i); // make room to fit data

		component->CopyData(*this, new_entity_id,
			&archetype->component_data[i][record.index * component_size],
			&archetype->component_data[i][current_size]);
	}

	archetype->entities.push_back(new_entity_id);

	new_record.index = IDType(archetype->entities.size() - 1);
	new_record.archetype = archetype;

	return new_entity_id;
}

void EntityAdmin::Shrink(bool extensive)
{
	if (m_component_lock)
		throw std::runtime_error("Components memory is currently locked from modifications");

	ClearEmptyEntityArchetypes();
	ClearEmptyTypeArchetypes();

	m_archetype_cache.clear();

	if (extensive) // shrink all archetypes data
	{
		for (std::size_t i = 0; i < m_archetypes.size(); ++i)
		{
			const ArchetypePtr& archetype = m_archetypes[i];
			const ComponentIDs& archetype_id = archetype->type;

			for (std::size_t j = 0; j < archetype_id.size(); ++j)
			{
				const auto component_id		= archetype->type[j];
				const auto component		= m_component_map[component_id].get();
				const auto component_size	= component->GetSize();

				const auto current_size		= archetype->entities.size() * component_size;
				const auto current_capacity = archetype->component_data_size[j];

				if (current_capacity > current_size)
				{
					archetype->component_data_size[j] = current_size;

					auto new_data = std::make_unique_for_overwrite<ByteArray>(archetype->component_data_size[j]);

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

void EntityAdmin::EraseComponentRef(EntityID entity_id, ComponentTypeID component_id) const
{
	const auto eit = m_entity_component_ref_map.find(entity_id);
	if (eit == m_entity_component_ref_map.end())
		return;

	const auto cit = eit->second.find(component_id);
	if (cit == eit->second.end())
		return;

	DataRef& ref = cit->second;
	switch (ref.flag)
	{
	case DataRef::R_Component:
	{
		if (ref.component_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.component_ptr.lock() = nullptr;
	}
	break;
	case DataRef::R_Base:
	{
		if (ref.base_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.base_ptr.lock() = nullptr;
	}
	break;
	case DataRef::R_All:
	{
		if (ref.component_ptr.expired() && ref.base_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else
		{
			*ref.component_ptr.lock() = nullptr;
			*ref.base_ptr.lock() = nullptr;
		}
	}
	break;
	}
}

void EntityAdmin::UpdateComponentRef(EntityID entity_id, ComponentTypeID component_id, void* new_component) const
{
	const auto eit = m_entity_component_ref_map.find(entity_id);
	if (eit == m_entity_component_ref_map.end())
		return;

	auto& component_map = eit->second;

	const auto cit = component_map.find(component_id);
	if (cit == component_map.end())
		return;

	DataRef& ref = cit->second;
	switch (ref.flag)
	{
	case DataRef::R_Component:
	{
		if (ref.component_ptr.expired())
		{
			component_map.erase(component_id);
		}
		else *ref.component_ptr.lock() = new_component;
	}
	break;
	case DataRef::R_Base:
	{
		if (ref.base_ptr.expired())
		{
			component_map.erase(component_id);
		}
		else *ref.base_ptr.lock() = reinterpret_cast<void*>((char*)new_component + ref.base_offset);
	}
	break;
	case DataRef::R_All:
	{
		if (ref.component_ptr.expired() && ref.base_ptr.expired())
		{
			component_map.erase(component_id);
		}
		else
		{
			*ref.component_ptr.lock() = new_component;
			*ref.base_ptr.lock() = reinterpret_cast<void*>((char*)new_component + ref.base_offset);
		}
	}
	break;
	}
}

void EntityAdmin::ClearEmptyEntityArchetypes()
{
	const auto [first, last] = std::ranges::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype == nullptr)
				return true;

			if (archetype->entities.empty())
			{
				for (const auto& pair : archetype->edges)
				{
					const auto& edge = pair.second;

					if (edge.add) edge.add->edges.erase(archetype->id);
					if (edge.rmv) edge.rmv->edges.erase(archetype->id);
				}

				m_archetype_map.erase(archetype->id);

				return true;
			}

			return false;
		});

	m_archetypes.erase(first, last); // remove any dangling archetypes with no entities
}
void EntityAdmin::ClearEmptyTypeArchetypes()
{
	const auto [first, last] = std::ranges::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype == nullptr)
				return true;

			if (archetype->type.empty())
			{
				for (const auto& pair : archetype->edges)
				{
					const auto& edge = pair.second;

					if (edge.add) edge.add->edges.erase(archetype->id);
					if (edge.rmv) edge.rmv->edges.erase(archetype->id);
				}

				m_archetype_map.erase(archetype->id);

				for (EntityID entity_id : archetype->entities)
				{
					m_entity_archetype_map.erase(entity_id);
					m_reusable_entity_ids.push(entity_id);
				}

				return true;
			}

			return false;
		});

	m_archetypes.erase(first, last); // remove any archetypes that holds no components (cant be used anyways), should not exist in the first place
}

void EntityAdmin::ConstructSwap(
	Archetype* new_archetype, Archetype* old_archetype,
	EntityID entity_id, const Record& record,
	EntityID last_entity_id, Record& last_record) const
{
	for (std::size_t i = 0, j = 0; i < new_archetype->type.size(); ++i)
	{
		const auto component_id		= new_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		const auto current_size		= new_archetype->entities.size() * component_size;
		const auto new_size			= current_size + component_size;

		if (new_size > new_archetype->component_data_size[i])
			MakeRoom(new_archetype, component, component_size, i);

		if (j < old_archetype->type.size() && component_id != old_archetype->type[j])
		{
			component->ConstructData(*this, entity_id, &new_archetype->component_data[i][current_size]);
		}
		else
		{
			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[j][record.index * component_size],
				&new_archetype->component_data[i][current_size]);

			component->MoveDestroyData(*this, last_entity_id,
				&old_archetype->component_data[j][last_record.index * component_size],
				&old_archetype->component_data[j][record.index * component_size]); // move data from last to current

			++j;
		}
	}

	old_archetype->entities[record.index] = last_entity_id;
	last_record.index = record.index;
}

void EntityAdmin::Construct(
	Archetype* new_archetype, Archetype* old_archetype, 
	EntityID entity_id, const Record& record) const
{
	const auto new_size = new_archetype->type.size();
	const auto old_size = old_archetype->type.size();

	for (std::size_t i = 0, j = 0; i < new_size; ++i)
	{
		const auto component_id		= new_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		const auto current_size		= new_archetype->entities.size() * component_size;
		const auto new_size			= current_size + component_size;

		if (new_size > new_archetype->component_data_size[i])
			MakeRoom(new_archetype, component, component_size, i);

		if (j == old_size || component_id != old_archetype->type[j])
		{
			component->ConstructData(*this, entity_id, &new_archetype->component_data[i][current_size]);
		}
		else
		{
			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[j][record.index * component_size],
				&new_archetype->component_data[i][current_size]);

			++j;
		}
	}
}

void EntityAdmin::DestructSwap(
	Archetype* old_archetype, Archetype* new_archetype, 
	EntityID entity_id, const Record& record,
	EntityID last_entity_id, Record& last_record) const
{
	const auto new_size = new_archetype->type.size();
	const auto old_size = old_archetype->type.size();

	for (std::size_t i = 0, j = 0; i < old_size; ++i) // we iterate over both archetypes
	{
		const auto component_id		= old_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		if (j == new_size || component_id != new_archetype->type[j])
		{
			component->DestroyData(*this, entity_id, &old_archetype->component_data[i][record.index * component_size]);
		}
		else
		{
			const auto current_size = new_archetype->entities.size() * component_size;
			const auto new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[j])
				MakeRoom(new_archetype, component, component_size, j); // make room to fit data

			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[i][record.index * component_size],
				&new_archetype->component_data[j][current_size]); // move all the valid data from old to new

			++j;
		}

		component->MoveDestroyData(*this, last_entity_id,
			&old_archetype->component_data[i][last_record.index * component_size],
			&old_archetype->component_data[i][record.index * component_size]); // move data to last
	}

	old_archetype->entities[record.index] = last_entity_id; // now swap ids
	last_record.index = record.index;
}

void EntityAdmin::Destruct(
	Archetype* old_archetype, Archetype* new_archetype, 
	EntityID entity_id, const Record& record) const
{
	const auto new_size = new_archetype->type.size();
	const auto old_size = old_archetype->type.size();

	for (std::size_t i = 0, j = 0; i < old_size; ++i) // we iterate over both archetypes
	{
		const auto component_id		= old_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		if (j == new_size || component_id != new_archetype->type[j]) // this is the component that should be destroyed
		{
			component->DestroyData(*this, entity_id, &old_archetype->component_data[i][record.index * component_size]);
		}
		else
		{
			const auto current_size = new_archetype->entities.size() * component_size;
			const auto new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[j])
				MakeRoom(new_archetype, component, component_size, j); // make room to fit data

			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[i][record.index * component_size],
				&new_archetype->component_data[j][current_size]); // move all the valid data from old to new

			++j;
		}
	}
}

void EntityAdmin::MakeRoom(Archetype* archetype, const IComponentAlloc* component, std::size_t data_size, std::size_t i) const
{
	std::size_t new_size = 2 * archetype->component_data_size[i] + data_size;
	auto new_data = std::make_unique<ByteArray>(new_size);

	for (std::size_t j = 0; j < archetype->entities.size(); ++j)
	{
		component->MoveDestroyData(*this, archetype->entities[j],
			&archetype->component_data[i][j * data_size],
			&new_data[j * data_size]);
	}

	archetype->component_data[i] = std::move(new_data);
	archetype->component_data_size[i] = new_size;
}

void EntityAdmin::Destroy()
{
	assert(!m_component_lock && "Destroy should not be called while iterating components ???");

	if (!m_destroyed)
	{
		for (const ArchetypePtr& archetype : m_archetypes)
		{
			for (std::size_t i = 0; i < archetype->type.size(); ++i)
			{
				const auto component_id = archetype->type[i];
				const auto component = m_component_map[component_id].get();
				const auto component_size = component->GetSize();

				for (std::size_t j = 0; j < archetype->entities.size(); ++j)
					component->Shutdown(*this, archetype->entities[j], &archetype->component_data[i][j * component_size]);
			}
		}

		if (m_shutdown) // additional cleanup if shutdown
		{
			m_entity_archetype_map.clear(); // deregister all entities
			m_archetypes.clear();
			m_archetype_cache.clear();
			m_systems.clear();

			for (auto& [entity_id, component_map] : m_entity_component_ref_map) // clear all references
			{
				for (auto& [component_id, ref] : component_map)
				{
					switch (ref.flag)
					{
					case DataRef::R_Component:
					{
						if (!ref.component_ptr.expired())
							*ref.component_ptr.lock() = nullptr;
					}
					break;
					case DataRef::R_Base:
					{
						if (!ref.base_ptr.expired())
							*ref.base_ptr.lock() = nullptr;
					}
					break;
					case DataRef::R_All:
					{
						if (!ref.component_ptr.expired() && !ref.base_ptr.expired())
						{
							*ref.component_ptr.lock() = nullptr;
							*ref.base_ptr.lock() = nullptr;
						}
					}
					break;
					}
				}
			}

			m_entity_component_ref_map.clear();
		}

		m_destroyed = true;
	}
}
