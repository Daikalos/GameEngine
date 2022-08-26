#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "Identifiers.h"
#include "Archetype.h"
#include "Component.h"
#include "System.hpp"

#include "../utilities/Time.hpp"

namespace vlx
{
	class ECS
	{
	private:
		using ComponentPtr = typename std::shared_ptr<ComponentBase>;
		using ArchetypePtr = typename std::shared_ptr<Archetype>;

		struct Record
		{
			Archetype* archetype{nullptr};
			std::size_t index{0}; // where in the ArchetypesArray is the entity located in
		};

		using CompnentTypeIDBaseMap = typename std::unordered_map<ComponentTypeID, ComponentPtr>;
		using EntityArchetypeMap	= typename std::unordered_map<EntityID, Record>;
		using ArchetypesArray		= typename std::vector<ArchetypePtr>;									// find matching archetype to update matching entities
		using SystemsArrayMap		= typename std::unordered_map<std::uint8_t, std::vector<SystemBase*>>;	// map layer to array of systems (layer allows for controlling the order of calls)

	public:
		ECS() : m_entity_id_counter(1) {}
		~ECS() {}

		EntityID GetNewId();

		void RunSystems(const std::uint8_t layer, Time& time);

		template<class C>
		void RegisterComponent();
		void RegisterSystem(const std::uint8_t layer, SystemBase* system);
		void RegisterEntity(const EntityID entityId);

		template<class C, typename... Args>
		C* AddComponent(const EntityID entityId, Args&&... args);
		template<class C>
		void RemoveComponent(const EntityID entityid);

		template<class C>
		bool IsComponentRegistered();
		template<class C>
		bool HasComponent(const EntityID entityId);

		template<class C>
		C* GetComponent(const EntityID entityId);

		template<class... Ts>
		std::vector<EntityID> GetEntitiesWith();

		Archetype* GetArchetype(const ArchetypeID& id);

	private:
		EntityID				m_entity_id_counter;
		EntityArchetypeMap		m_entity_archetype_map;
		ArchetypesArray			m_archetypes;			// find matching archetype to update matching entities
		SystemsArrayMap			m_systems;				// map layer to array of systems (layer allows for controlling the order of calls)
		CompnentTypeIDBaseMap	m_component_map;
	};

	inline EntityID ECS::GetNewId()
	{
		return m_entity_id_counter++;
	}

	inline void ECS::RunSystems(const std::uint8_t layer, Time& time)
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

	template<class C>
	inline void ECS::RegisterComponent()
	{
		ComponentTypeID component_type_id = Component<C>::get_type_id();

		if (m_component_map.contains(component_type_id))
			return;

		m_component_map.emplace(component_type_id, comp_ptr(new Component<C>));
	}
	inline void ECS::RegisterSystem(const std::uint8_t layer, SystemBase* system)
	{
		m_systems[layer].push_back(system);
	}
	inline void ECS::RegisterEntity(const EntityID entityId)
	{
		m_entity_archetype_map[entityId] = Record();
	}

	template<class C, typename ...Args>
	inline C* ECS::AddComponent(const EntityID entityId, Args && ...args)
	{
		ComponentTypeID new_comp_type_id = Component<C>::GetTypeId();

		assert(is_component_registered<C>()); // component should be registered

		const std::size_t& comp_data_size = m_component_map[new_comp_type_id]->GetSize();

		Record& record = m_entity_archetype_map[entityId];
		Archetype* old_archetype = record.archetype;

		C* new_component = nullptr;
		Archetype* new_archetype = nullptr;

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			// cant add multiple of the same component to same entity
			assert(std::find(old_archetype->m_type.begin(), old_archetype->m_type.end(), new_comp_type_id) == old_archetype->m_type.end());

			ArchetypeID new_archetype_id = old_archetype->m_type;	// creates new archetype id
			new_archetype_id.push_back(new_comp_type_id);			// adds the component to this new archetype

			std::sort(new_archetype_id.begin(), new_archetype_id.end());

			new_archetype = GetArchetype(new_archetype_id);

			for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // for every component in the new archetype
			{
				const ComponentTypeID& new_comp_id = new_archetype_id[i];
				const ComponentBase* const new_comp = m_component_map[new_comp_id];
				const std::size_t& new_comp_data_size = new_comp->GetSize();

				std::size_t current_size = new_archetype->m_entitity_ids.size() * new_comp_data_size;
				std::size_t new_size = current_size + new_comp_data_size;

				if (new_size > new_archetype->m_component_data_size[i]) // make room to fit old data
				{
					new_archetype->m_component_data_size[i] *= 2;
					new_archetype->m_component_data_size[i] += new_comp_data_size;

					ComponentData new_data = ComponentData(new unsigned char[new_archetype->m_component_data_size[i]]);

					for (std::size_t j = 0; j < new_archetype->m_entitity_ids.size(); ++j)
					{
						new_comp->MoveData(
							&new_archetype->m_component_data[i][j * comp_data_size], 
							&new_data[j * comp_data_size]);
						new_comp->DestroyData(&new_archetype->m_component_data[i][j * comp_data_size]);
					}

					new_archetype->m_component_data[i] = new_data;
				}

				for (std::size_t j = 0; j < old_archetype->m_type.size(); ++i) // move data from old to new if it exists
				{
					const ComponentTypeID& old_comp_id = old_archetype->m_type[i];
					if (old_comp_id == new_comp_id)
					{
						const ComponentBase* const old_comp = m_component_map[old_comp_id];
						const std::size_t& old_comp_data_size = old_comp->GetSize();

						old_comp->MoveData(
							&old_archetype->m_component_data[j][record.index * old_comp_data_size],
							&new_archetype->m_component_data[i][current_size]);
						old_comp->DestroyData(&old_archetype->m_component_data[j][record.index * old_comp_data_size]);

						break;
					}
				}

				// ...
			}
		}
		else
		{
			ArchetypeID new_archetype_id(1, new_comp_type_id);
			new_archetype = get_archetype(new_archetype_id); // construct new archetype using the id

			const ComponentBase* const new_comp = m_component_map[new_comp_type_id];

			std::size_t current_size = new_archetype->m_entitity_ids.size() * comp_data_size;
			std::size_t new_size = current_size + comp_data_size;

			if (new_size > new_archetype->m_component_data_size[0]) // make room and move over existing data
			{
				new_archetype->m_component_data_size[0] *= 2;
				new_archetype->m_component_data_size[0] += comp_data_size;

				ComponentData new_data = ComponentData(new unsigned char[new_archetype->m_component_data_size[0]]);

				for (std::size_t i = 0; i < new_archetype->m_entitity_ids.size(); ++i)
				{
					new_comp->MoveData(
						&new_archetype->m_component_data[0][i * comp_data_size], 
						&new_data[i * comp_data_size]);
					new_comp->DestroyData(&new_archetype->m_component_data[0][i * comp_data_size]);
				}

				new_archetype->m_component_data[0] = new_data;
			}

			new_component = new (&new_archetype->m_component_data[0][current_size]) C(std::forward<Args>(args)...);
		}

		new_archetype->m_entitity_ids.push_back(entityId);
		record.index = new_archetype->m_entitity_ids.size() - 1;
		record.archetype = new_archetype;

		return new_component;
	}

	template<class C>
	inline void ECS::RemoveComponent(const EntityID entityid)
	{

	}

	template<class C>
	inline bool ECS::IsComponentRegistered()
	{
		return m_component_map.contains(Component<C>::GetTypeId());
	}
	template<class C>
	inline bool ECS::HasComponent(const EntityID entityId)
	{
		return false;
	}

	inline Archetype* ECS::GetArchetype(const ArchetypeID& id)
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

		return m_archetypes.emplace_back(new_archetype).get();
	}

	template<class C>
	inline C* ECS::GetComponent(const EntityID entityId)
	{
		return nullptr;
	}

	template<class ...Ts>
	inline std::vector<EntityID> ECS::GetEntitiesWith()
	{
		return std::vector<EntityID>();
	}
}