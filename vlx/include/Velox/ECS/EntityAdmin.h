#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"
#include "Component.hpp"
#include "System.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// 
	// Based on article by Deckhead:
	// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/ 
	// 
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// 
	// Data-oriented ECS design. Be warned that adding and removing components
	// is an expensive operation. Try to perform operations on startup as much as possible
	// and look at using pooling instead to prevent removing and adding entities often.
	// 
	////////////////////////////////////////////////////////////
	class EntityAdmin
	{
	private:
		using ComponentPtr = typename std::unique_ptr<ComponentBase>;
		using ArchetypePtr = typename std::unique_ptr<Archetype>;

		struct Record
		{
			Archetype*	archetype	{nullptr};
			std::size_t index		{0}; // where in the archetype entity array is the entity located at
		};

		using ComponentTypeIDBaseMap = typename std::unordered_map<ComponentTypeID, ComponentPtr>;
		using EntityArchetypeMap	= typename std::unordered_map<EntityID, Record>;
		using ArchetypesArray		= typename std::vector<ArchetypePtr>; // find matching archetype to update matching entities
		using SystemsArrayMap		= typename std::unordered_map<std::uint16_t, std::vector<SystemBase*>>; // map layer to array of systems (layer allows for controlling the order of calls)

	public:
		VELOX_API EntityAdmin();
		VELOX_API ~EntityAdmin();

		VELOX_API EntityID GetNewId();

		VELOX_API void RunSystems(const std::uint8_t layer, Time& time);
		VELOX_API void SortSystems(const std::uint8_t layer);

		VELOX_API void RegisterSystem(const std::uint8_t layer, SystemBase* system);
		VELOX_API void RegisterEntity(const EntityID entity_id);

		VELOX_API void RemoveSystem(const std::uint16_t layer, SystemBase* system);
		VELOX_API void RemoveEntity(const EntityID entity_id);

		/// <summary>
		///		Shrinks the ECS by removing all the empty archetypes
		/// </summary>
		/// <param name="extensive">- 
		///		perform a complete shrink of the ECS by removing all the extra data space, will most likely 
		///		invalidate all the existing pointers from e.g., GetComponent()
		/// </param>
		VELOX_API void Shrink(bool extensive = false);

		template<class C> requires IsComponentType<C>
		void RegisterComponent();

		template<class C> requires IsComponentType<C>
		bool IsComponentRegistered();
		template<class C> requires IsComponentType<C>
		bool HasComponent(const EntityID entity_id);

		template<class C> requires IsComponentType<C>
		C* GetComponent(const EntityID entity_id);

		template<Exists... Cs> requires IsComponentType<Cs...>
		std::vector<EntityID> GetEntitiesWith();

		template<class C, typename... Args> requires IsComponentType<C> && std::constructible_from<C, Args...>
		C* AddComponent(const EntityID entity_id, Args&&... args);
		template<class C> requires IsComponentType<C>
		void RemoveComponent(const EntityID entity_id);

	private:
		VELOX_API Archetype* GetArchetype(const ArchetypeID& id);

		////////////////////////////////////////////////////////////
		// Helper functions
		////////////////////////////////////////////////////////////

		VELOX_API void MakeRoom(
			Archetype* archetype,
			const ComponentBase* component,
			const std::size_t data_size, 
			const std::size_t i);

		VELOX_API bool MoveComponent(
			Archetype* old_archetype,
			Archetype* new_archetype,
			const ComponentBase* component,
			const std::size_t component_id,
			const std::size_t old_component,
			const std::size_t new_component,
			const std::size_t i);

	private:
		EntityID				m_entity_id_counter;
		EntityArchetypeMap		m_entity_archetype_map;
		ArchetypesArray			m_archetypes;			// find matching archetype to update matching entities
		SystemsArrayMap			m_systems;				// map layer to array of systems (layer allows for controlling the order of calls)
		ComponentTypeIDBaseMap	m_component_map;
	};

	template<class C> requires IsComponentType<C>
	inline void EntityAdmin::RegisterComponent()
	{
		ComponentTypeID component_type_id = Component<C>::GetTypeId();

		if (m_component_map.contains(component_type_id))
			return;

		m_component_map.emplace(component_type_id, std::make_unique<Component<C>>());
	}

	template<class C> requires IsComponentType<C>
	inline bool EntityAdmin::IsComponentRegistered()
	{
		return m_component_map.contains(Component<C>::GetTypeId());
	}
	template<class C> requires IsComponentType<C>
	inline bool EntityAdmin::HasComponent(const EntityID entity_id)
	{
		return GetComponent<C>(entity_id) != nullptr;
	}

	template<class C> requires IsComponentType<C>
	inline C* EntityAdmin::GetComponent(const EntityID entity_id)
	{
		if (!IsComponentRegistered<C>())
			return nullptr;

		auto it = m_entity_archetype_map.find(entity_id);

		if (it == m_entity_archetype_map.end())
			return nullptr;

		Record& record = it->second;
		Archetype* archetype = record.archetype;

		if (!archetype)
			return nullptr;

		const ComponentTypeID component_id = Component<C>::GetTypeId();

		const ArchetypeID& archetype_id = archetype->type;
		for (std::size_t i = 0; i < archetype_id.size(); ++i)
		{
			const ComponentTypeID& id = archetype_id[i];
			if (id == component_id)
			{
				C* components = reinterpret_cast<C*>(&archetype->component_data[i][0]);
				return &components[record.index];
			}
		}

		return nullptr;
	}

	template<Exists... Cs> requires IsComponentType<Cs...>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith()
	{
		std::vector<EntityID> entities;

		std::vector<std::uint32_t> archetypes;
		std::size_t total_size = 0;

		ArchetypeID component_ids = SortKeys({ { Component<Cs>::GetTypeId()... } }); // see system.hpp

		for (std::size_t i = 0; i < m_archetypes.size(); ++i)
		{
			const ArchetypePtr& archetype = m_archetypes[i];
			const ArchetypeID& archetype_id = archetype->type;

			if (std::includes(component_ids.begin(), component_ids.end(), archetype_id.begin(), archetype_id.end()))
			{
				archetypes.push_back(i);
				total_size += archetype->entities.size();
			}
		}

		entities.reserve(total_size);
		for (const std::uint32_t i : archetypes)
		{
			const auto& archetype_entities = m_archetypes[i]->entities;

			entities.insert(entities.end(),
				archetype_entities.begin(),
				archetype_entities.end());
		}

		return entities;
	}

	template<class C, typename ...Args> requires IsComponentType<C> && std::constructible_from<C, Args...>
	inline C* EntityAdmin::AddComponent(const EntityID entity_id, Args&&... args)
	{
		if (IsComponentRegistered<C>()) // component should be registered
			return nullptr;

		ComponentTypeID new_component_id = Component<C>::GetTypeId();

		Record& record = m_entity_archetype_map[entity_id];
		Archetype* old_archetype = record.archetype;

		C* new_component = nullptr;
		Archetype* new_archetype = nullptr;

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			if (std::find(old_archetype->type.begin(), old_archetype->type.end(), new_component_id) != old_archetype->type.end()) // already contains component
				return nullptr;

			ArchetypeID new_archetype_id = old_archetype->type;	  // create copy
			cu::InsertSorted(new_archetype_id, new_component_id); // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)

			new_archetype = GetArchetype(new_archetype_id);

			for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // move all the data from old to new
			{
				const ComponentTypeID& component_id = new_archetype_id[i];
				const ComponentBase* component = m_component_map[component_id].get();
				const std::size_t& component_size = component->GetSize();

				const std::size_t current_size = new_archetype->entities.size() * component_size;
				const std::size_t new_size = current_size + component_size;

				if (new_size > new_archetype->component_data_size[i])
					MakeRoom(new_archetype, component, component_size, i);

				bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

				if (!found)
				{
					assert(new_component == nullptr); // should never happen twice

					new_component = new (&new_archetype->component_data[i][current_size])
						C(std::forward<Args>(args)...);
				}
			}

			assert(new_component != nullptr); // a new component should have been added

			EntityID last_entity_id = old_archetype->entities.back();
			Record& last_record = m_entity_archetype_map[last_entity_id];

			if (last_entity_id != entity_id) // no point of swapping data with itself
			{
				ArchetypeID& old_archetype_id = old_archetype->type;
				for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // move the last entity's data to current entity
				{
					const ComponentTypeID& component_id = old_archetype_id[i];
					const ComponentBase* component = m_component_map[component_id].get();
					const std::size_t& component_size = component->GetSize();

					component->MoveDestroyData(
						&old_archetype->component_data[i][last_record.index * component_size],
						&old_archetype->component_data[i][record.index * component_size]); // move data to last
				}

				old_archetype->entities.at(record.index) = old_archetype->entities.back(); // now swap ids (using *.at() because the flow is slightly confusing)
				last_record.index = record.index;
			}

			old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next compoonent is added, it will overwrite the previous
		}
		else // if the entity has no archetype, first component
		{
			ArchetypeID new_archetype_id(1, new_component_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id);		// construct or get new archetype using the id

			const ComponentBase* component = m_component_map[new_component_id].get();
			const std::size_t& component_size = component->GetSize();

			const std::size_t current_size = new_archetype->entities.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[0])
				MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

			new_component = new (&new_archetype->component_data[0][current_size])
				C(std::forward<Args>(args)...);
		}

		new_archetype->entities.push_back(entity_id);
		record.index = new_archetype->entities.size() - 1;
		record.archetype = new_archetype;

		return new_component;
	}

	template<class C> requires IsComponentType<C>
	inline void EntityAdmin::RemoveComponent(const EntityID entity_id)
	{
		if (!IsComponentRegistered<C>())
			return;

		auto eit = m_entity_archetype_map.find(entity_id);

		if (eit == m_entity_archetype_map.end())
			return;

		Record& record = eit->second;
		Archetype* old_archetype = record.archetype;

		if (!old_archetype)
			return;

		ComponentTypeID old_component_id = Component<C>::GetTypeId();

		if (std::find(old_archetype->type.begin(), old_archetype->type.end(), old_component_id) == old_archetype->type.end()) // if the component does not exist, just exit
			return;

		ArchetypeID new_archetype_id = old_archetype->type;
		cu::Erase(new_archetype_id, old_component_id);

		Archetype* new_archetype = GetArchetype(new_archetype_id);

		for (std::size_t i = 0; i < new_archetype_id.size(); ++i)
		{
			const ComponentTypeID& component_id = new_archetype_id[i];
			const ComponentBase* component = m_component_map[component_id].get();
			const std::size_t& component_size = component->GetSize();

			const std::size_t current_size = new_archetype->entities.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[i])
				MakeRoom(new_archetype, component, component_size, i); // make room to fit data

			MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);
		}

		EntityID last_entity_id = old_archetype->entities.back();
		Record& last_record = m_entity_archetype_map[last_entity_id];

		if (last_entity_id != entity_id) // no point of swapping data with itself
		{
			ArchetypeID& old_archetype_id = old_archetype->type;
			for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // move the last entity's data to current entity
			{
				const ComponentTypeID& component_id = old_archetype_id[i];
				const ComponentBase* component = m_component_map[component_id].get();
				const std::size_t& component_size = component->GetSize();

				if (component_id == old_component_id)
					component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);

				component->MoveDestroyData(
					&old_archetype->component_data[i][last_record.index * component_size],
					&old_archetype->component_data[i][record.index * component_size]); // move data to last
			}

			old_archetype->entities.at(record.index) = old_archetype->entities.back(); // now swap ids
			last_record.index = record.index;
		}

		old_archetype->entities.pop_back();

		new_archetype->entities.push_back(entity_id);
		record.index = new_archetype->entities.size() - 1;
		record.archetype = new_archetype;
	}
}






// -- old add component --

//// cant add multiple of the same component to same entity
//if (std::find(old_archetype->type.begin(), old_archetype->type.end(), new_comp_type_id) != old_archetype->type.end())
//	return nullptr;

//ArchetypeID new_archetype_id = old_archetype->type;	  // create copy
//cu::InsertSorted(new_archetype_id, new_comp_type_id); // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)

//new_archetype = GetArchetype(new_archetype_id); // create the new archetype

//for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // for every component in the new archetype
//{
//	const ComponentTypeID& component_id = new_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	const std::size_t current_size = new_archetype->entities.size() * component_size;
//	const std::size_t new_size = current_size + component_size;

//	if (new_size > new_archetype->component_data_size[i])
//		MakeRoom(new_archetype, component, component_size, i); // make room to fit old data

//	bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

//	if (!found) // if there was no old instance we could copy our data to, construct a new component
//	{
//		new_component = new (&new_archetype->component_data[i][current_size])
//			C(std::forward<Args>(args)...);
//	}
//}

//const ArchetypeID& old_archetype_id = old_archetype->type;
//for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // we'll have to remove this entity's data in the old as it has been assigned a new one
//{
//	const ComponentTypeID& component_id = old_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();

//	EraseComponent(old_archetype, component, record.index, i);
//}

//auto it = std::find(old_archetype->entities.begin(), old_archetype->entities.end(), entity_id); // look at swap and pop

//std::for_each(it, old_archetype->entities.end(),
//	[this, &old_archetype](const EntityID& eid)
//	{
//		--m_entity_archetype_map[eid].index;
//	});

//old_archetype->entities.erase(it); // may result in dangling empty archetypes, but is better than having to continously reallocate new memory



// -- old remove component --

//ArchetypeID new_archetype_id = old_archetype->type; // create copy

//new_archetype_id.erase(std::remove(
//	new_archetype_id.begin(), new_archetype_id.end(), comp_type_id), new_archetype_id.end()); // remove this component

////std::sort(new_archetype_id.begin(), new_archetype_id.end()); should not be needed

//Archetype* new_archetype = GetArchetype(new_archetype_id);

//for (std::size_t i = 0; i < new_archetype_id.size(); ++i)
//{
//	const ComponentTypeID& component_id = new_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	const std::size_t current_size = new_archetype->entities.size() * component_size;
//	const std::size_t new_size = current_size + component_size;

//	if (new_size > new_archetype->component_data_size[i])
//		MakeRoom(new_archetype, component, component_size, i); // make room to fit data

//	component->ConstructData(&new_archetype->component_data[i][current_size]);

//	bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

//	assert(found);
//}

//const ArchetypeID& old_archetype_id = old_archetype->type;
//for (std::size_t i = 0; i < old_archetype_id.size(); ++i)
//{
//	const ComponentTypeID& component_id = old_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();

//	if (component_id == comp_type_id)
//	{
//		const std::size_t component_size = component->GetSize();
//		component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);
//	}

//	EraseComponent(old_archetype, component, record.index, i);
//}

//auto it = std::find(
//	old_archetype->entities.begin(),
//	old_archetype->entities.end(), entity_id);

//std::for_each(it, old_archetype->entities.end(),
//	[this](const EntityID& eid)
//	{
//		--m_entity_archetype_map[eid].index;
//	});

//old_archetype->entities.erase(std::remove(
//	old_archetype->entities.begin(), old_archetype->entities.end(), entity_id),
//	old_archetype->entities.end());

//new_archetype->entities.push_back(entity_id);
//record.index = new_archetype->entities.size() - 1;
//record.archetype = new_archetype;