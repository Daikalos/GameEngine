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
	// Based on article by Deckhead:
	// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/ 
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
			std::size_t index		{0}; // where in the archetype array is the entity located in
		};

		using CompnentTypeIDBaseMap = typename std::unordered_map<ComponentTypeID, ComponentPtr>;
		using EntityArchetypeMap	= typename std::unordered_map<EntityID, Record>;
		using ArchetypesArray		= typename std::vector<ArchetypePtr>;									// find matching archetype to update matching entities
		using SystemsArrayMap		= typename std::unordered_map<std::uint8_t, std::vector<SystemBase*>>;	// map layer to array of systems (layer allows for controlling the order of calls)

	public:
		VELOX_API EntityAdmin();
		VELOX_API ~EntityAdmin();

		VELOX_API EntityID GetNewId();

		VELOX_API void RunSystems(const std::uint8_t layer, Time& time);

		VELOX_API void RegisterSystem(const std::uint8_t layer, SystemBase* system);
		VELOX_API void RegisterEntity(const EntityID entity_id);

		VELOX_API void RemoveEntity(const EntityID entity_id);

		template<class C>
		void RegisterComponent();

		template<class C>
		bool IsComponentRegistered();
		template<class C>
		bool HasComponent(const EntityID entity_id);

		template<class C>
		C* GetComponent(const EntityID entity_id);

		template<class C, typename... Args>
		C* AddComponent(const EntityID entity_id, Args&&... args) requires std::constructible_from<C, Args...>;
		template<class C>
		void RemoveComponent(const EntityID entity_id);

		template<class... Ts>
		std::vector<EntityID> GetEntitiesWith();

	private:
		VELOX_API Archetype* GetArchetype(const ArchetypeID& id);

		////////////////////////////////////////////////////////////
		// Helper function for increasing the capacity
		////////////////////////////////////////////////////////////
		VELOX_API void MakeRoom(Archetype* new_archetype, const ComponentBase* new_comp, const size_t data_size, const size_t i);

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
		return nullptr;
	}

	template<class C, typename ...Args>
	inline C* EntityAdmin::AddComponent(const EntityID entity_id, Args&&... args) requires std::constructible_from<C, Args...>
	{
		ComponentTypeID new_comp_type_id = Component<C>::GetTypeId();

		if (!IsComponentRegistered<C>()) // component should be registered
		{
			throw std::runtime_error("Component is not registered");
			return nullptr;
		}

		const std::size_t& comp_data_size = m_component_map[new_comp_type_id]->GetSize();

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
				const ComponentTypeID& new_comp_id = new_archetype_id[i];
				const ComponentBase* new_comp = m_component_map[new_comp_id].get();
				const std::size_t& new_comp_data_size = new_comp->GetSize();

				const std::size_t current_size = new_archetype->m_entity_ids.size() * new_comp_data_size;
				const std::size_t new_size = current_size + new_comp_data_size;

				if (new_size > new_archetype->m_component_data_size[i]) // make room to fit old data
					MakeRoom(new_archetype, new_comp, new_comp_data_size, i);

				bool old_found = false;
				for (std::size_t j = 0; j < old_archetype->m_type.size(); ++j) // only move data from old to new if it exists
				{
					const ComponentTypeID& old_comp_id = old_archetype->m_type[j];
					if (old_comp_id == new_comp_id)
					{
						const ComponentBase* old_comp = m_component_map[old_comp_id].get();
						const std::size_t& old_comp_data_size = old_comp->GetSize();

						old_comp->MoveDestroyData(
							&old_archetype->m_component_data[j][record.index * old_comp_data_size],
							&new_archetype->m_component_data[i][current_size]);

						old_found = true;

						break;
					}
				}

				if (!old_found) // if there was no old instance we could copy our data to, construct a new component
				{
					new_component = new (&new_archetype->m_component_data[i][current_size])
						C(std::forward<Args>(args)...);
				}
			}

			const ArchetypeID& old_archetype_id = old_archetype->m_type;

			for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // we'll have to remove this entity as it has been assigned a new one
			{
				const ComponentTypeID& old_comp_type_id = old_archetype_id[i];

				const ComponentBase* old_comp = m_component_map[old_comp_type_id].get();
				const std::size_t& old_comp_data_size = old_comp->GetSize();

				ComponentData new_data = std::make_unique<ByteArray>(old_archetype->m_component_data_size[i] - old_comp_data_size);
				old_archetype->m_component_data_size[i] -= old_comp_data_size;

				for (std::size_t j = 0, ri = 0; j < old_archetype->m_entity_ids.size(); ++j)
				{
					if (j == record.index)
						continue;

					old_comp->MoveDestroyData(
						&old_archetype->m_component_data[i][j * old_comp_data_size],
						&new_data[ri * old_comp_data_size]);

					++ri;
				}

				old_archetype->m_component_data[i] = std::move(new_data);
			}

			auto it = std::find(old_archetype->m_entity_ids.begin(), old_archetype->m_entity_ids.end(), entity_id);

			std::for_each(it, old_archetype->m_entity_ids.end(),
				[this, &old_archetype](const EntityID& eid)
				{
					--m_entity_archetype_map[eid].index;
				});

			old_archetype->m_entity_ids.erase(it);
		}
		else // if the entity has no archetype, first component
		{
			ArchetypeID new_archetype_id(1, new_comp_type_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id);		// construct new archetype using the id

			const ComponentBase* new_comp = m_component_map[new_comp_type_id].get();

			const std::size_t current_size = new_archetype->m_entity_ids.size() * comp_data_size;
			const std::size_t new_size = current_size + comp_data_size;

			if (new_size > new_archetype->m_component_data_size[0]) // make room and move over existing data
				MakeRoom(new_archetype, new_comp, comp_data_size, 0);

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
			const ComponentTypeID& new_comp_id = new_archetype_id[i];
			const ComponentBase* new_comp = m_component_map[new_comp_id].get();
			const std::size_t& new_comp_data_size = new_comp->GetSize();

			const std::size_t current_size = new_archetype->m_entity_ids.size() * new_comp_data_size;
			const std::size_t new_size = current_size + new_comp_data_size;

			if (new_size > new_archetype->m_component_data_size[i]) // make room to fit data
				MakeRoom(new_archetype, new_comp, new_comp_data_size, i);

			new_comp->ConstructData(&new_archetype->m_component_data[i][current_size]);

			for (std::size_t j = 0; j < old_archetype->m_type.size(); ++j)
			{
				const ComponentTypeID& old_comp_id = old_archetype->m_type[j];
				if (old_comp_id == new_comp_id)
				{
					const ComponentBase* old_comp = m_component_map[old_comp_id].get();
					const std::size_t& old_comp_data_size = old_comp->GetSize();

					old_comp->MoveDestroyData(
						&old_archetype->m_component_data[j][record.index * old_comp_data_size],
						&new_archetype->m_component_data[i][current_size]);

					break;
				}
			}
		}

		const ArchetypeID& old_archetype_id = old_archetype->m_type;

		for (std::size_t i = 0; i < old_archetype_id.size(); ++i)
		{
			const ComponentTypeID& old_comp_type_id = old_archetype_id[i];

			if (old_comp_type_id == comp_type_id)
			{
				const ComponentBase* comp = m_component_map[comp_type_id].get();
				comp->DestroyData(&old_archetype->m_component_data[i][record.index * sizeof(C)]);
			}

			const ComponentBase* old_comp = m_component_map[old_comp_type_id].get();
			const std::size_t old_comp_data_size = old_comp->GetSize();

			std::size_t current_size = old_archetype->m_entity_ids.size() * old_comp_data_size;
			std::size_t new_size = current_size - old_comp_data_size;

			ComponentData new_data = std::make_unique<ByteArray>(old_archetype->m_component_data_size[i] - old_comp_data_size);
			old_archetype->m_component_data_size[i] -= old_comp_data_size;

			for (std::size_t j = 0, ri = 0; j < old_archetype->m_entity_ids.size(); ++j)
			{
				if (j == record.index)
					continue;

				old_comp->MoveDestroyData(
					&old_archetype->m_component_data[i][j * old_comp_data_size],
					&new_data[ri * old_comp_data_size]);

				++ri;
			}

			old_archetype->m_component_data[i] = std::move(new_data);
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

	template<class ...Ts>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith()
	{
		return std::vector<EntityID>();
	}
}