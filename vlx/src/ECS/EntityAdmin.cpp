#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

EntityAdmin::EntityAdmin() : m_entity_id_counter(1) { }

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

	auto insert = m_entity_archetype_map.try_emplace(entity_id, Record());
	assert(insert.second);

	return insert.first->second;
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
		const auto& archtypes = GetArchetypes(system->GetArchKey(), system->GetIDKey());

		if (system->IsRunningParallel())
		{
			std::for_each(std::execution::par_unseq, archtypes.begin(), archtypes.end(),
				[&system](Archetype* archetype)
				{
					system->DoAction(archetype);
				});
		}
		else
		{
			std::for_each(std::execution::unseq, archtypes.begin(), archtypes.end(),
				[&system](Archetype* archetype)
				{
					system->DoAction(archetype);
				});
		}
	}
}

void EntityAdmin::SortSystems(const LayerType layer)
{
	const auto it = m_systems.find(layer);
	if (it == m_systems.end())
		return;

	std::stable_sort(it->second.begin(), it->second.end(),
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
	const auto eit = m_entity_archetype_map.find(entity_id);
	if (eit == m_entity_archetype_map.end())
		return false;

	Record& record		 = eit->second;
	Archetype* archetype = record.archetype;

	if (archetype == nullptr)
	{
		m_entity_archetype_map.erase(entity_id);
		m_reusable_entity_ids.push(entity_id);

		return true;
	}

	EntityID last_entity_id = archetype->entities.back();
	Record& last_record		= m_entity_archetype_map[last_entity_id];

	const bool same_entity = (last_entity_id == entity_id);

	const ComponentIDs& archetype_id = archetype->type;
	for (std::size_t i = 0; i < archetype_id.size(); ++i) // we iterate over both archetypes
	{
		const ComponentTypeID component_id	= archetype_id[i];
		const IComponentAlloc* component	= m_component_map[component_id].get();
		const std::size_t& component_size	= component->GetSize();

		component->DestroyData(*this, entity_id, &archetype->component_data[i][record.index * component_size]);

		if (!same_entity)
		{
			component->MoveDestroyData(*this, last_entity_id,
				&archetype->component_data[i][last_record.index * component_size],
				&archetype->component_data[i][record.index * component_size]);
		}
	}

	if (!same_entity)
	{
		archetype->entities[record.index] = archetype->entities.back(); // now swap ids with last
		last_record.index = record.index;
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

			assert(new_archetype_id != old_archetype->type);
			assert(new_archetype_id == new_archetype->type);
		}
		else new_archetype = ait->second.add;

		const EntityID last_entity_id = old_archetype->entities.back();
		Record& last_record = m_entity_archetype_map[last_entity_id];

		const bool same_entity = (last_entity_id == entity_id);

		for (std::size_t i = 0, j = 0; i < new_archetype->type.size(); ++i) // move all the data from old to new and perform swaps at the same time
		{
			const auto component_id		= new_archetype->type[i];
			const auto component		= m_component_map[component_id].get();
			const auto component_size	= component->GetSize();

			const auto current_size	= new_archetype->entities.size() * component_size;
			const auto new_size		= current_size + component_size;

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

				if (!same_entity)
				{
					component->MoveDestroyData(*this, last_entity_id,
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

	const auto ait = old_archetype->edges.find(archetype_id);
	if (ait == old_archetype->edges.end())
	{
		ComponentIDs new_archetype_id = old_archetype->type; // create copy

		bool found = false;
		for (const ComponentTypeID component_id : component_ids) // determine valid components
		{
			if (cu::EraseSorted(new_archetype_id, component_id)) // erase while keeping the vector sorted (this should ensure that the archetype is always sorted)
				found = true;
		}

		if (!found) // unable to add any component
			return false;

		new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentIDs>()(new_archetype_id));
		old_archetype->edges[archetype_id].remove = new_archetype;

		assert(new_archetype_id != old_archetype->type);
		assert(new_archetype_id == new_archetype->type);
	}
	else new_archetype = ait->second.remove;

	const EntityID last_entity_id = old_archetype->entities.back();
	Record& last_record = m_entity_archetype_map[last_entity_id];

	const bool same_entity = (last_entity_id == entity_id);

	for (std::size_t i = 0, j = 0; i < old_archetype->type.size(); ++i) // we iterate over both archetypes
	{
		const auto component_id		= old_archetype->type[i];
		const auto component		= m_component_map[component_id].get();
		const auto component_size	= component->GetSize();

		if (component_id != new_archetype->type[j]) // this is the component that should be destroyed
		{
			component->DestroyData(*this, entity_id, &old_archetype->component_data[i][record.index * component_size]);
		}
		else
		{
			const auto current_size = new_archetype->entities.size() * component_size;
			const auto new_size		= current_size + component_size;

			if (new_size > new_archetype->component_data_size[j])
				MakeRoom(new_archetype, component, component_size, j); // make room to fit data

			component->MoveDestroyData(*this, entity_id,
				&old_archetype->component_data[i][record.index * component_size],
				&new_archetype->component_data[j][current_size]); // move all the valid data from old to new

			++j;
		}

		if (!same_entity) // no point of swapping data with itself
		{
			component->MoveDestroyData(*this, last_entity_id,
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

	record.index = IDType(new_archetype->entities.size() - 1);
	record.archetype = new_archetype;

	return true;
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
		const ComponentTypeID component_id	= archetype->type[i];
		const IComponentAlloc* component	= m_component_map[component_id].get();
		const auto& component_size			= component->GetSize();

		const auto current_size = std::uint32_t(archetype->entities.size()) * component_size;
		const auto new_size = current_size + component_size;

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
	// remove any dangling archetypes with no entities
	m_archetypes.erase(std::remove_if(m_archetypes.begin(), m_archetypes.end(),
		[this](const ArchetypePtr& archetype)
		{
			if (archetype->entities.empty())
			{
				m_archetype_map.erase(archetype->id);
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
				m_archetype_map.erase(archetype->id);

				for (const EntityID entity_id : archetype->entities)
				{
					m_entity_archetype_map.erase(entity_id);
					m_reusable_entity_ids.push(entity_id);
				}

				return true;
			}

			return false;
		}), m_archetypes.end());

	m_archetype_cache.clear();

	if (extensive) // shrink all archetypes data
	{
		for (std::size_t i = 0; i < m_archetypes.size(); ++i)
		{
			const ArchetypePtr& archetype = m_archetypes[i];
			const ComponentIDs& archetype_id = archetype->type;

			for (std::size_t j = 0; j < archetype_id.size(); ++j)
			{
				const ComponentTypeID& component_id = archetype->type[i];
				const IComponentAlloc* component = m_component_map[component_id].get();
				const auto& component_size = component->GetSize();

				const auto current_size = std::uint32_t(archetype->entities.size()) * component_size;
				const auto current_capacity = archetype->component_data_size[j];

				if (current_capacity > current_size)
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

void EntityAdmin::EraseComponentRef(const EntityID entity_id, const ComponentTypeID component_id) const
{
	const auto eit = m_entity_component_ref_map.find(entity_id);
	if (eit == m_entity_component_ref_map.end())
		return;

	const auto cit = eit->second.find(component_id);
	if (cit == eit->second.end())
		return;

	DataRef& data_ref = cit->second;
	switch (data_ref.flag)
	{
	case RefFlag::Component:
	{
		if (data_ref.component_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *data_ref.component_ptr.lock() = nullptr;
	}
	break;
	case RefFlag::Base:
	{
		if (data_ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *data_ref.base.ptr.lock() = nullptr;
	}
	break;
	case RefFlag::All:
	{
		if (data_ref.component_ptr.expired() && data_ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else
		{
			*data_ref.component_ptr.lock() = nullptr;
			*data_ref.base.ptr.lock() = nullptr;
		}
	}
	break;
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

	DataRef& data_ref = cit->second;
	switch (data_ref.flag)
	{
	case RefFlag::Component:
	{
		if (data_ref.component_ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *data_ref.component_ptr.lock() = new_component;
	}
	break;
	case RefFlag::Base:
	{
		if (data_ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else *data_ref.base.ptr.lock() = reinterpret_cast<void*>(new_component + data_ref.base.offset);
	}
	break;
	case RefFlag::All:
	{
		if (data_ref.component_ptr.expired() && data_ref.base.ptr.expired())
		{
			eit->second.erase(component_id);
		}
		else
		{
			*data_ref.component_ptr.lock() = new_component;
			*data_ref.base.ptr.lock() = reinterpret_cast<void*>(new_component + data_ref.base.offset);
		}
	}
	break;
	}
}

void EntityAdmin::MakeRoom(Archetype* archetype, const IComponentAlloc* component, const std::size_t data_size, const std::size_t i)
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