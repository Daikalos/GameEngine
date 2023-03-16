#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

EntityAdmin::~EntityAdmin()
{
	for (const auto& archetype : m_archetypes)
	{
		for (std::size_t i = 0; i < archetype->type.size(); ++i)
		{
			const ComponentTypeID& component_id = archetype->type[i];
			const IComponentAlloc* component	= m_component_map[component_id].get();
			const std::size_t& component_size	= component->GetSize();

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
				component->Shutdown(*this, archetype->entities[j], &archetype->component_data[i][j * component_size]);
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

bool EntityAdmin::IsEntityRegistered(const EntityID entity_id) const
{
	return m_entity_archetype_map.contains(entity_id);
}
bool EntityAdmin::HasComponent(const EntityID entity_id, const ComponentTypeID component_id) const
{
	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return false;

	const Archetype* archetype = eit->second.archetype;

	if (!archetype)
		return false;

	const auto cit = m_component_archetypes_map.find(component_id);
	if (cit == m_component_archetypes_map.end())
		return false;

	return cit->second.contains(archetype->id);
}

auto EntityAdmin::RegisterEntity(const EntityID entity_id) -> Record&
{
	assert(entity_id != NULL_ENTITY);

	auto [it, success] = m_entity_archetype_map.try_emplace(entity_id, Record());
	assert(success);

	return it->second;
}
void EntityAdmin::RegisterSystem(const LayerType layer, ISystem* system)
{
	m_systems[layer].push_back(system);
}

void EntityAdmin::RunSystems(const LayerType layer) const
{
	const auto sit = m_systems.find(layer);
	if (sit == m_systems.end())
		return;

	for (const ISystem* system : sit->second)
	{
		const auto& archetypes = GetArchetypes(system->GetArchKey(), system->GetIDKey());

		system->Start();

		if (system->IsRunningParallel())
		{
			std::for_each(std::execution::par_unseq, archetypes.begin(), archetypes.end(),
				[&system](const Archetype* const archetype)
				{
					system->Run(archetype);
				});
		}
		else
		{
			std::for_each(std::execution::unseq, archetypes.begin(), archetypes.end(),
				[&system](const Archetype* const archetype)
				{
					system->Run(archetype);
				});
		}
	}
}

void EntityAdmin::SortSystems(const LayerType layer)
{
	const auto it = m_systems.find(layer);
	if (it == m_systems.end())
		return;

	std::ranges::stable_sort(it->second.begin(), it->second.end(),
		[](const ISystem* lhs, const ISystem* rhs)
		{
			return *lhs > *rhs;
		});
}

void EntityAdmin::RemoveSystem(const LayerType layer, ISystem* system)
{
	if (!cu::Erase(m_systems[layer], system))
		throw std::runtime_error("attempted removal of non-existing system");
}

bool EntityAdmin::RemoveEntity(const EntityID entity_id)
{
	// TODO: prevent RemoveEntity being called on shutdown

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

	const EntityID last_entity_id = archetype->entities.back();

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

void EntityAdmin::AddComponent(const EntityID entity_id, const ComponentTypeID add_component_id)
{
	const auto component_ids = { add_component_id };
	const auto archetype_id = cu::ContainerHash<ComponentIDs>()(component_ids);

	AddComponents(entity_id, component_ids, archetype_id);
}

bool EntityAdmin::RemoveComponent(const EntityID entity_id, const ComponentTypeID rmv_component_id)
{
	const auto component_ids = { rmv_component_id };
	const auto archetype_id = cu::ContainerHash<ComponentIDs>()(component_ids);

	return RemoveComponents(entity_id, component_ids, archetype_id);
}

void EntityAdmin::AddComponents(const EntityID entity_id, const ComponentIDs& component_ids, const ArchetypeID archetype_id)
{
	assert(cu::IsSorted(component_ids) && !component_ids.empty());

	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return;

	Record& record = eit->second;
	Archetype* old_archetype = record.archetype;

	Archetype* new_archetype = nullptr; // we are going to be moving to a new archetype

	if (old_archetype) // already has an attached archetype, define a new archetype
	{
		const auto ait = old_archetype->edges.find(archetype_id);
		if (ait == old_archetype->edges.end())
		{
			ComponentIDs new_archetype_id = old_archetype->type; // create copy

			bool found = false;
			for (const ComponentTypeID component_id : component_ids) // determine valid components
			{
				if (cu::InsertSorted(new_archetype_id, component_id)) // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)
					found = true;
			}

			if (!found) // unable to add any component
				return;

			new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentIDs>()(new_archetype_id));

			old_archetype->edges[archetype_id].add = new_archetype;
			new_archetype->edges[archetype_id].remove = old_archetype;

			assert(new_archetype_id != old_archetype->type);
			assert(new_archetype_id == new_archetype->type);
		}
		else new_archetype = ait->second.add;

		const EntityID last_entity_id = old_archetype->entities.back();

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

bool EntityAdmin::RemoveComponents(const EntityID entity_id, const ComponentIDs& component_ids, const ArchetypeID archetype_id)
{
	assert(cu::IsSorted(component_ids) && !component_ids.empty());

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
		for (const ComponentTypeID component_id : component_ids) // erase while keeping the vector sorted
		{
			if (cu::EraseSorted(new_archetype_id, component_id))
				found = true;
		}

		if (!found) // unable to remove any component
			return false;

		new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentIDs>()(new_archetype_id));

		new_archetype->edges[archetype_id].add = old_archetype;
		old_archetype->edges[archetype_id].remove = new_archetype;
	}
	else new_archetype = ait->second.remove;

	const EntityID last_entity_id = old_archetype->entities.back();

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

bool EntityAdmin::HasShutdown() const
{
	return m_shutdown;
}

void EntityAdmin::Shutdown()
{
	m_shutdown = true;
}

EntityID EntityAdmin::Duplicate(const EntityID entity_id)
{
	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return NULL_ENTITY;

	Record& record = eit->second;
	Archetype* archetype = record.archetype;

	if (archetype == nullptr)
		return NULL_ENTITY;

	const EntityID new_entity_id = GetNewEntityID();
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

					auto new_data = std::make_unique<ByteArray>(archetype->component_data_size[j]);

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

void EntityAdmin::EraseComponentRef(const EntityID entity_id, const ComponentTypeID component_id) const
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
	case RefFlag::Component:
	{
		if (ref.component_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.component_ptr.lock() = nullptr;
	}
	break;
	case RefFlag::Base:
	{
		if (ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.base.ptr.lock() = nullptr;
	}
	break;
	case RefFlag::All:
	{
		if (ref.component_ptr.expired() && ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else
		{
			*ref.component_ptr.lock() = nullptr;
			*ref.base.ptr.lock() = nullptr;
		}
	}
	break;
	default: break;
	}
}

void EntityAdmin::UpdateComponentRef(const EntityID entity_id, const ComponentTypeID component_id, IComponent* new_component) const
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
	case RefFlag::Component:
	{
		if (ref.component_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.component_ptr.lock() = new_component;
	}
	break;
	case RefFlag::Base:
	{
		if (ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *ref.base.ptr.lock() = reinterpret_cast<void*>(new_component + ref.base.offset);
	}
	break;
	case RefFlag::All:
	{
		if (ref.component_ptr.expired() && ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else
		{
			*ref.component_ptr.lock() = new_component;
			*ref.base.ptr.lock() = reinterpret_cast<void*>(new_component + ref.base.offset);
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
			if (archetype->entities.empty())
			{
				// TODO: Remove edges on neighbouring archetypes that references this

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
			if (archetype->type.empty())
			{
				// TODO: Remove edges on neighbouring archetypes that references this

				m_archetype_map.erase(archetype->id);

				for (const EntityID entity_id : archetype->entities)
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
	const EntityID entity_id, const Record& record,
	const EntityID last_entity_id, Record& last_record) const
{
	for (std::size_t i = 0, j = 0; i < new_archetype->type.size(); ++i)
	{
		const auto component_id = new_archetype->type[i];
		const auto component = m_component_map.at(component_id).get();
		const auto component_size = component->GetSize();

		const auto current_size = new_archetype->entities.size() * component_size;
		const auto new_size = current_size + component_size;

		if (new_size > new_archetype->component_data_size[i])
			MakeRoom(new_archetype, component, component_size, i);

		if (component_id != old_archetype->type[j])
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
	const EntityID entity_id, const Record& record) const
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

		if (component_id != old_archetype->type[j])
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
	const EntityID entity_id, const Record& record,
	const EntityID last_entity_id, Record& last_record) const
{
	for (std::size_t i = 0, j = 0; i < old_archetype->type.size(); ++i) // we iterate over both archetypes
	{
		const auto component_id		= old_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		if (component_id != new_archetype->type[j]) // this is the component that should be destroyed
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
	const EntityID entity_id, const Record& record) const
{
	for (std::size_t i = 0, j = 0; i < old_archetype->type.size(); ++i) // we iterate over both archetypes
	{
		const auto component_id		= old_archetype->type[i];
		const auto component		= m_component_map.at(component_id).get();
		const auto component_size	= component->GetSize();

		if (component_id != new_archetype->type[j]) // this is the component that should be destroyed
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

void EntityAdmin::MakeRoom(Archetype* archetype, const IComponentAlloc* component, const std::size_t data_size, const std::size_t i) const
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