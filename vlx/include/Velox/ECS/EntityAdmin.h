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

		using CompnentTypeIDBaseMap = typename std::unordered_map<ComponentTypeID, ComponentPtr>;
		using EntityArchetypeMap	= typename std::unordered_map<EntityID, Record>;
		using ArchetypesArray		= typename std::vector<ArchetypePtr>; // find matching archetype to update matching entities
		using SystemsArrayMap		= typename std::unordered_map<std::uint8_t, std::vector<SystemBase*>>; // map layer to array of systems (layer allows for controlling the order of calls)

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

		template<class C>
		void RegisterComponent();

		template<class C>
		bool IsComponentRegistered();
		template<class C>
		bool HasComponent(const EntityID entity_id);

		template<class C>
		C* GetComponent(const EntityID entity_id);

		template<class C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponent(const EntityID entity_id, Args&&... args);
		template<class C>
		void RemoveComponent(const EntityID entity_id);

		template<class... Cs>
		std::vector<EntityID> GetEntitiesWith();

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

		VELOX_API void EraseComponent(
			Archetype* archetype, 
			const ComponentBase* component,
			const std::size_t record_index, 
			const std::size_t i);

	private:
		EntityID				m_entity_id_counter;
		EntityArchetypeMap		m_entity_archetype_map;
		ArchetypesArray			m_archetypes;			// find matching archetype to update matching entities
		SystemsArrayMap			m_systems;				// map layer to array of systems (layer allows for controlling the order of calls)
		CompnentTypeIDBaseMap	m_component_map;
	};

	template<class C>
	inline void EntityAdmin::RegisterComponent()
	{
		ComponentTypeID component_type_id = Component<C>::GetTypeId();

		if (m_component_map.contains(component_type_id))
			return;

		m_component_map.emplace(component_type_id, std::make_unique<Component<C>>());
	}

	template<class C>
	inline bool EntityAdmin::IsComponentRegistered()
	{
		return m_component_map.contains(Component<C>::GetTypeId());
	}
	template<class C>
	inline bool EntityAdmin::HasComponent(const EntityID entity_id)
	{
		return false;
	}

	template<class C>
	inline C* EntityAdmin::GetComponent(const EntityID entity_id)
	{
		Record& record = m_entity_archetype_map[entity_id];
		Archetype* archetype = record.archetype;

		const ComponentTypeID id = Component<C>::GetTypeId();

		for (const ComponentTypeID& comp_id : archetype->m_type)
		{
			if (id == comp_id)
			{

			}
		}

		return nullptr;
	}

	template<class C, typename ...Args> requires std::constructible_from<C, Args...>
	inline C* EntityAdmin::AddComponent(const EntityID entity_id, Args&&... args)
	{
		if (!IsComponentRegistered<C>()) // component should be registered
			return nullptr;

		ComponentTypeID new_comp_type_id = Component<C>::GetTypeId();

		Record& record = m_entity_archetype_map[entity_id];
		Archetype* old_archetype = record.archetype;

		C* new_component = nullptr;
		Archetype* new_archetype = nullptr;

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			// cant add multiple of the same component to same entity
			if (std::find(old_archetype->m_type.begin(), old_archetype->m_type.end(), new_comp_type_id) != old_archetype->m_type.end())
				return nullptr;

			ArchetypeID new_archetype_id = old_archetype->m_type;			// create copy
			new_archetype_id.push_back(new_comp_type_id);					// adds the new component to this new archetype
			std::sort(new_archetype_id.begin(), new_archetype_id.end());

			new_archetype = GetArchetype(new_archetype_id); // create the new archetype

			for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // for every component in the new archetype
			{
				const ComponentTypeID& component_id = new_archetype_id[i];
				const ComponentBase* component = m_component_map[component_id].get();
				const std::size_t& component_size = component->GetSize();

				const std::size_t current_size = new_archetype->m_entity_ids.size() * component_size;
				const std::size_t new_size = current_size + component_size;

				if (new_size > new_archetype->m_component_data_size[i])
					MakeRoom(new_archetype, component, component_size, i); // make room to fit old data

				bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

				if (!found) // if there was no old instance we could copy our data to, construct a new component
				{
					new_component = new (&new_archetype->m_component_data[i][current_size])
						C(std::forward<Args>(args)...);
				}
			}

			const ArchetypeID& old_archetype_id = old_archetype->m_type;
			for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // we'll have to remove this entity's data in the old as it has been assigned a new one
			{
				const ComponentTypeID& component_id = old_archetype_id[i];
				const ComponentBase* component = m_component_map[component_id].get();

				EraseComponent(old_archetype, component, record.index, i);
			}

			auto it = std::find(old_archetype->m_entity_ids.begin(), old_archetype->m_entity_ids.end(), entity_id);

			std::for_each(it, old_archetype->m_entity_ids.end(),
				[this, &old_archetype](const EntityID& eid)
				{
					--m_entity_archetype_map[eid].index;
				});

			old_archetype->m_entity_ids.erase(it); // may result in dangling empty archetypes, but is better than having to continously reallocate new memory
		}
		else // if the entity has no archetype, first component
		{
			ArchetypeID new_archetype_id(1, new_comp_type_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id);		// construct new archetype using the id

			const ComponentBase* component = m_component_map[new_comp_type_id].get();
			const std::size_t& component_size = component->GetSize();

			const std::size_t current_size = new_archetype->m_entity_ids.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->m_component_data_size[0])
				MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

			new_component = new (&new_archetype->m_component_data[0][current_size])
				C(std::forward<Args>(args)...);
		}

		new_archetype->m_entity_ids.push_back(entity_id);
		record.index = new_archetype->m_entity_ids.size() - 1;
		record.archetype = new_archetype;

		return new_component;
	}

	template<class C>
	inline void EntityAdmin::RemoveComponent(const EntityID entity_id)
	{
		if (!IsComponentRegistered<C>())
			return;

		if (!m_entity_archetype_map.contains(entity_id))
			return;

		ComponentTypeID comp_type_id = Component<C>::GetTypeId();

		Record& record = m_entity_archetype_map[entity_id];
		Archetype* old_archetype = record.archetype;

		if (!old_archetype)
			return;

		if (std::find(old_archetype->m_type.begin(), old_archetype->m_type.end(), comp_type_id) == old_archetype->m_type.end())
			return;

		ArchetypeID new_archetype_id = old_archetype->m_type; // create copy

		new_archetype_id.erase(std::remove(
			new_archetype_id.begin(), new_archetype_id.end(), comp_type_id), new_archetype_id.end()); // remove this component

		std::sort(new_archetype_id.begin(), new_archetype_id.end());

		Archetype* new_archetype = GetArchetype(new_archetype_id);

		for (std::size_t i = 0; i < new_archetype_id.size(); ++i)
		{
			const ComponentTypeID& component_id = new_archetype_id[i];
			const ComponentBase* component = m_component_map[component_id].get();
			const std::size_t& component_size = component->GetSize();

			const std::size_t current_size = new_archetype->m_entity_ids.size() * component_size;
			const std::size_t new_size = current_size + component_size;

			if (new_size > new_archetype->m_component_data_size[i])
				MakeRoom(new_archetype, component, component_size, i); // make room to fit data

			component->ConstructData(&new_archetype->m_component_data[i][current_size]);

			assert(MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i));
		}

		const ArchetypeID& old_archetype_id = old_archetype->m_type;
		for (std::size_t i = 0; i < old_archetype_id.size(); ++i)
		{
			const ComponentTypeID& component_id = old_archetype_id[i];
			const ComponentBase* component = m_component_map[component_id].get();

			if (component_id == comp_type_id)
			{
				const std::size_t component_size = component->GetSize();
				component->DestroyData(&old_archetype->m_component_data[i][record.index * component_size]);
			}

			EraseComponent(old_archetype, component, record.index, i);
		}

		auto it = std::find(
			old_archetype->m_entity_ids.begin(),
			old_archetype->m_entity_ids.end(), entity_id);

		std::for_each(it, old_archetype->m_entity_ids.end(),
			[this, &old_archetype](const EntityID& eid)
			{
				--m_entity_archetype_map[eid].index;
			});

		old_archetype->m_entity_ids.erase(std::remove(
			old_archetype->m_entity_ids.begin(), old_archetype->m_entity_ids.end(), entity_id),
			old_archetype->m_entity_ids.end());

		new_archetype->m_entity_ids.push_back(entity_id);
		record.index = new_archetype->m_entity_ids.size() - 1;
		record.archetype = new_archetype;
	}

	template<class ...Cs>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith()
	{
		return std::vector<EntityID>();
	}
}