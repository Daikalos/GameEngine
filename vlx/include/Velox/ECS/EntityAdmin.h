#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <queue>

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

		struct ArchetypeRecord
		{
			std::size_t column{0};
		};

		using ComponentTypeIDBaseMap	= typename std::unordered_map<ComponentTypeID, ComponentPtr>;
		using ComponentArchetypesMap	= typename std::unordered_map<ComponentTypeID, std::unordered_set<IDType>>;
		using EntityArchetypeMap		= typename std::unordered_map<EntityID, Record>;
		using ArchetypeMap				= typename std::unordered_map<ArchetypeID, Archetype*, cu::VectorHash<ArchetypeID>>;
		using ArchetypesArray			= typename std::vector<ArchetypePtr>; // all the existing archetypes
		using SystemsArrayMap			= typename std::unordered_map<std::uint16_t, std::vector<SystemBase*>>; // map layer to array of systems (layer allows for controlling the order of calls)

	public:
		VELOX_API EntityAdmin();
		VELOX_API ~EntityAdmin();

	public:
		template<IsComponentType C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponent(const EntityID entity_id, Args&&... args);
		template<IsComponentType C>
		void RemoveComponent(const EntityID entity_id);

		template<IsComponentType C>
		void RegisterComponent();

		template<IsComponentType C>
		bool IsComponentRegistered();
		template<IsComponentType C>
		bool HasComponent(const EntityID entity_id);

		template<IsComponentType C>
		C* GetComponent(const EntityID entity_id);

		template<IsComponentType... Cs> requires Exists<Cs...>
		std::vector<EntityID> GetEntitiesWith();

		template<IsComponentType... Cs> requires Exists<Cs...>
		void Reserve(const std::size_t component_count);

	public:
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

		VELOX_API void Sort();

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

	private:
		EntityID				m_entity_id_counter;
		std::queue<EntityID>	m_reusable_ids;

		EntityArchetypeMap		m_entity_archetype_map;
		ComponentArchetypesMap	m_component_archetypes_map;
		ArchetypeMap			m_archetype_map;
		ArchetypesArray			m_archetypes;			// find matching archetype to update matching entities
		SystemsArrayMap			m_systems;				// map layer to array of systems (layer allows for controlling the order of calls)
		ComponentTypeIDBaseMap	m_component_map;
	};

	template<IsComponentType C, typename... Args> requires std::constructible_from<C, Args...>
	inline C* EntityAdmin::AddComponent(const EntityID entity_id, Args&&... args)
	{
		if (!IsComponentRegistered<C>()) // component should be registered
			return nullptr;

		Record& record = m_entity_archetype_map[entity_id];
		Archetype* old_archetype = record.archetype;

		C* add_component = nullptr;
		Archetype* new_archetype = nullptr;

		const ComponentTypeID add_component_id = Component<C>::GetTypeId();

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			if (std::find(old_archetype->type.begin(), old_archetype->type.end(), add_component_id) != old_archetype->type.end()) // already contains component
				return nullptr;

			ArchetypeID new_archetype_id = old_archetype->type;	  // create copy
			cu::InsertSorted(new_archetype_id, add_component_id); // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)

			new_archetype = GetArchetype(new_archetype_id);

			for (std::size_t i = 0, j = 0; i < new_archetype_id.size(); ++i) // move all the data from old to new
			{
				const ComponentTypeID& component_id = new_archetype_id[i];
				const ComponentBase* component = m_component_map[component_id].get();
				const std::size_t& component_size = component->GetSize();

				const std::size_t current_size = new_archetype->entities.size() * component_size;
				const std::size_t new_size = current_size + component_size;

				if (new_size > new_archetype->component_data_size[i])
					MakeRoom(new_archetype, component, component_size, i);

				if (component_id == add_component_id)
				{
					assert(add_component == nullptr); // should never happen twice

					add_component = new (&new_archetype->component_data[i][current_size])
						C(std::forward<Args>(args)...);

					continue;
				}

				component->MoveDestroyData(
					&old_archetype->component_data[j][record.index * component_size],
					&new_archetype->component_data[i][current_size]);

				++j;
			}

			assert(add_component != nullptr); // a new component should have been added

			EntityID last_entity_id = old_archetype->entities.back();
			Record& last_record = m_entity_archetype_map[last_entity_id];

			if (last_entity_id != entity_id) // no point of swapping data with itself
			{
				ArchetypeID& old_archetype_id = old_archetype->type;
				for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // move the last entity's data to current entity
				{
					const ComponentTypeID& old_component_id = old_archetype_id[i];
					const ComponentBase* old_component = m_component_map[old_component_id].get();
					const std::size_t& old_component_size = old_component->GetSize();

					old_component->MoveDestroyData(
						&old_archetype->component_data[i][last_record.index * old_component_size],
						&old_archetype->component_data[i][record.index * old_component_size]); // move data to last
				}

				old_archetype->entities.at(record.index) = old_archetype->entities.back(); // now swap ids (using *.at() because the flow is slightly confusing)
				last_record.index = record.index;
			}

			old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next component is added, it will overwrite the previous
		}
		else // if the entity has no archetype, first component
		{
			ArchetypeID new_archetype_id(1, add_component_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id);		// construct or get new archetype using the id

			const ComponentBase* component = m_component_map[add_component_id].get();
			const std::size_t& component_size = component->GetSize();

			const std::size_t current_size = new_archetype->entities.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[0])
				MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

			add_component = new (&new_archetype->component_data[0][current_size])
				C(std::forward<Args>(args)...);
		}

		new_archetype->entities.push_back(entity_id);

		record.index = new_archetype->entities.size() - 1;
		record.archetype = new_archetype;

		return add_component;
	}

	template<IsComponentType C>
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

		const ComponentTypeID rmv_component_id = Component<C>::GetTypeId();

		if (std::find(old_archetype->type.begin(), old_archetype->type.end(), rmv_component_id) == old_archetype->type.end()) // if the component does not exist, just exit
			return;

		const ArchetypeID& old_archetype_id = old_archetype->type;

		ArchetypeID new_archetype_id = old_archetype->type;
		cu::Erase(new_archetype_id, rmv_component_id);

		Archetype* new_archetype = GetArchetype(new_archetype_id);

		for (std::size_t i = 0, j = 0; i < old_archetype_id.size(); ++i) // we iterate over both archetypes
		{
			const ComponentTypeID component_id = old_archetype_id[i];
			const ComponentBase* component = m_component_map[component_id].get();
			const std::size_t component_size = component->GetSize();

			if (component_id == rmv_component_id)
			{
				component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);
				continue;
			}

			const std::size_t current_size = new_archetype->entities.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->component_data_size[j])
				MakeRoom(new_archetype, component, component_size, j); // make room to fit data

			component->MoveDestroyData(
				&old_archetype->component_data[i][record.index * component_size],
				&new_archetype->component_data[j][current_size]); // move all the valid data from old to new

			++j;
		}

		EntityID last_entity_id = old_archetype->entities.back();
		Record& last_record = m_entity_archetype_map[last_entity_id];

		if (last_entity_id != entity_id) // no point of swapping data with itself
		{
			ArchetypeID& old_archetype_id = old_archetype->type;
			for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // move the last entity's data to current entity
			{
				const ComponentTypeID old_component_id = old_archetype_id[i];
				const ComponentBase* old_component = m_component_map[old_component_id].get();
				const std::size_t old_component_size = old_component->GetSize();

				old_component->MoveDestroyData(
					&old_archetype->component_data[i][last_record.index * old_component_size],
					&old_archetype->component_data[i][record.index * old_component_size]); // move data to last
			}

			old_archetype->entities.at(record.index) = old_archetype->entities.back(); // now swap ids
			last_record.index = record.index;
		}

		old_archetype->entities.pop_back();

		new_archetype->entities.push_back(entity_id);
		record.index = new_archetype->entities.size() - 1;
		record.archetype = new_archetype;
	}

	template<IsComponentType C>
	inline void EntityAdmin::RegisterComponent()
	{
		ComponentTypeID component_type_id = Component<C>::GetTypeId();

		if (m_component_map.contains(component_type_id))
			return;

		m_component_map.emplace(component_type_id, std::make_unique<Component<C>>());
	}

	template<IsComponentType C>
	inline bool EntityAdmin::IsComponentRegistered()
	{
		return m_component_map.contains(Component<C>::GetTypeId());
	}

	template<IsComponentType C>
	inline bool EntityAdmin::HasComponent(const EntityID entity_id)
	{
		if (!IsComponentRegistered<C>())
			return false;

		auto it = m_entity_archetype_map.find(entity_id);

		if (it == m_entity_archetype_map.end())
			return false;

		Record& record = it->second;
		Archetype* archetype = record.archetype;

		if (!archetype)
			return false;

		const ComponentTypeID component_id = Component<C>::GetTypeId();
		for (const ComponentTypeID& id : archetype->type) // basically the same as GetComponent, except without the casts
		{
			if (id == component_id)
				return true;
		}

		return false;
	}

	template<IsComponentType C>
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

	template<IsComponentType... Cs> requires Exists<Cs...>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith()
	{
		std::vector<EntityID> entities;

		ArchetypeID component_ids = SortKeys({ { Component<Cs>::GetTypeId()... } }); // see system.hpp

		std::vector<std::uint32_t> archetypes; // list of matching archetypes
		std::size_t total_size = 0; // used to prevent numerous reallocations

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

	template<IsComponentType... Cs> requires Exists<Cs...>
	inline void EntityAdmin::Reserve(const std::size_t component_count)
	{
		ArchetypeID component_ids = SortKeys({ { Component<Cs>::GetTypeId()... } }); // see system.hpp

		for (std::size_t i = 0; i < m_archetypes.size(); ++i)
		{
			const ArchetypePtr& archetype = m_archetypes[i];
			const ArchetypeID& archetype_id = archetype->type;

			auto it = std::search(archetype_id.begin(), archetype_id.end(), component_ids.begin(), component_ids.end());

			std::size_t j = it - archetype_id.begin();
			std::int64_t count = std::ssize(component_ids);

			for (; count > 0 && j < archetype_id.size(); ++j, --count)
			{
				const ComponentTypeID& component_id = archetype_id[j];
				const ComponentBase* component = m_component_map[component_id];
				const std::size_t& component_size = component->GetSize();

				const std::size_t current_size = archetype->entities.size() * component_size;
				const std::size_t new_size = component_count * component_size;

				if (new_size > current_size)
				{
					archetype->component_data_size[j] = new_size;
					ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[j]);

					for (std::size_t k = 0; k < archetype->entities.size(); ++k)
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