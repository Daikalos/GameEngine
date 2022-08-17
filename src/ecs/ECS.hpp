#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "Identifiers.h"
#include "Archetype.h"
#include "Component.h"
#include "System.hpp"

namespace fge
{
	class ECS
	{
	private:
		using ComponentPtr = std::shared_ptr<ComponentBase>;
		using ArchetypePtr = std::shared_ptr<Archetype>;

		struct Record
		{
			Archetype* archetype{nullptr};
			std::size_t index{0}; // where in the ArchetypesArray is the entity located in
		};

		using CompnentTypeIDBaseMap = std::unordered_map<ComponentTypeID, ComponentPtr>;
		using EntityArchetypeMap = std::unordered_map<EntityID, Record>;				   
		using ArchetypesArray = std::vector<ArchetypePtr>;										// find matching archetype to update matching entities
		using SystemsArrayMap = std::unordered_map<std::uint8_t, std::vector<SystemBase*>>; // map layer to array of systems (layer allows for controlling the order of calls)

	public:
		ECS() : _entity_id_counter(1) {}
		~ECS() {}

		EntityID get_new_id();

		void run_systems(const std::uint8_t layer, const float dt);

		template<class C>
		void register_component();
		void register_system(const std::uint8_t layer, SystemBase* system);
		void register_entity(const EntityID entityId);

		template<class C, typename... Args>
		C* add_component(const EntityID entityId, Args&&... args);
		template<class C>
		void remove_component(const EntityID entityid);

		template<class C>
		bool is_component_registered();
		template<class C>
		bool has_component(const EntityID entityId);

		template<class C>
		C* get_component(const EntityID entityId);

		template<class... Ts>
		std::vector<EntityID> get_entities_with();

		Archetype* get_archetype(const ArchetypeID& id);

	private:
		EntityID _entity_id_counter;
		EntityArchetypeMap _entity_archetype_map;
		ArchetypesArray _archetypes;
		SystemsArrayMap _systems;
		CompnentTypeIDBaseMap _component_map;
	};

	inline EntityID ECS::get_new_id()
	{
		return _entity_id_counter++;
	}

	inline void ECS::run_systems(const std::uint8_t layer, const float dt)
	{
		for (SystemBase* system : _systems[layer])
		{
			const ArchetypeID& key = system->get_key();
			for (const ArchetypePtr& archetype : _archetypes)
			{
				if (std::includes(archetype->_type.begin(), archetype->_type.end(), key.begin(), key.end()))
				{
					system->update(dt, archetype.get());
				}
			}
		}
	}

	template<class C>
	inline void ECS::register_component()
	{
		ComponentTypeID componentTypeId = Component<C>::get_type_id();

		if (_component_map.contains(componentTypeId))
			return;

		_component_map.emplace(componentTypeId, comp_ptr(new Component<C>));
	}
	inline void ECS::register_system(const std::uint8_t layer, SystemBase* system)
	{
		_systems[layer].push_back(system);
	}
	inline void ECS::register_entity(const EntityID entityId)
	{
		_entity_archetype_map[entityId] = Record();
	}

	template<class C, typename ...Args>
	inline C* ECS::add_component(const EntityID entityId, Args && ...args)
	{
		ComponentTypeID new_comp_type_id = Component<C>::get_type_id();

		assert(is_component_registered<C>()); // component should be registered

		const std::size_t& comp_data_size = _component_map[new_comp_type_id]->get_size();

		Record& record = _entity_archetype_map[entityId];
		Archetype* old_archetype = record.archetype;

		C* new_component = nullptr;
		Archetype* new_archetype = nullptr;

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			// cant add multiple of the same component to same entity
			assert(std::find(old_archetype->_type.begin(), old_archetype->_type.end(), new_comp_type_id) == old_archetype->_type.end());

			ArchetypeID new_archetype_id = old_archetype->_type; // creates new archetype id
			new_archetype_id.push_back(new_comp_type_id);		 // adds the component to this new archetype

			std::sort(new_archetype_id.begin(), new_archetype_id.end());

			new_archetype = get_archetype(new_archetype_id);

			for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // for every component in the new archetype
			{
				const ComponentTypeID& new_comp_id = new_archetype_id[i];
				const ComponentBase* const new_comp = _component_map[new_comp_id];
				const std::size_t& new_comp_data_size = new_comp->get_size();

				std::size_t current_size = new_archetype->_entitity_ids.size() * new_comp_data_size;
				std::size_t new_size = current_size + new_comp_data_size;

				if (new_size > new_archetype->_component_data_size[i]) // make room to fit old data
				{
					new_archetype->_component_data_size[i] *= 2;
					new_archetype->_component_data_size[i] += new_comp_data_size;

					ComponentData new_data = ComponentData(new unsigned char[new_archetype->_component_data_size[i]]);

					for (std::size_t j = 0; j < new_archetype->_entitity_ids.size(); ++j)
					{
						new_comp->move_data(
							&new_archetype->_component_data[i][j * comp_data_size], 
							&new_data[j * comp_data_size]);
						new_comp->destroy_data(&new_archetype->_component_data[i][j * comp_data_size]);
					}

					new_archetype->_component_data[i] = new_data;
				}

				for (std::size_t j = 0; j < old_archetype->_type.size(); ++i) // move data from old to new if it exists
				{
					const ComponentTypeID& old_comp_id = old_archetype->_type[i];
					if (old_comp_id == new_comp_id)
					{
						const ComponentBase* const old_comp = _component_map[old_comp_id];
						const std::size_t& old_comp_data_size = old_comp->get_size();

						old_comp->move_data(
							&old_archetype->_component_data[j][record.index * old_comp_data_size],
							&new_archetype->_component_data[i][current_size]);
						old_comp->destroy_data(&old_archetype->_component_data[j][record.index * old_comp_data_size]);

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

			const ComponentBase* const new_comp = _component_map[new_comp_type_id];

			std::size_t current_size = new_archetype->_entitity_ids.size() * comp_data_size;
			std::size_t new_size = current_size + comp_data_size;

			if (new_size > new_archetype->_component_data_size[0]) // make room and move over existing data
			{
				new_archetype->_component_data_size[0] *= 2;
				new_archetype->_component_data_size[0] += comp_data_size;

				ComponentData new_data = ComponentData(new unsigned char[new_archetype->_component_data_size[0]]);

				for (std::size_t i = 0; i < new_archetype->_entitity_ids.size(); ++i)
				{
					new_comp->move_data(
						&new_archetype->_component_data[0][i * comp_data_size], 
						&new_data[i * comp_data_size]);
					new_comp->destroy_data(&new_archetype->_component_data[0][i * comp_data_size]);
				}

				new_archetype->_component_data[0] = new_data;
			}

			new_component = new (&new_archetype->_component_data[0][current_size]) C(std::forward<Args>(args)...);
		}

		new_archetype->_entitity_ids.push_back(entityId);
		record.index = new_archetype->_entitity_ids.size() - 1;
		record.archetype = new_archetype;

		return new_component;
	}

	template<class C>
	inline void ECS::remove_component(const EntityID entityid)
	{

	}

	template<class C>
	inline bool ECS::is_component_registered()
	{
		return _component_map.contains(Component<C>::get_type_id());
	}
	template<class C>
	inline bool ECS::has_component(const EntityID entityId)
	{
		return false;
	}

	inline Archetype* ECS::get_archetype(const ArchetypeID& id)
	{
		for (const ArchetypePtr& archetype : _archetypes)
		{
			if (archetype->_type == id)
				return archetype.get();
		}

		// archetype does not exist, create new one

		ArchetypePtr new_archetype = ArchetypePtr(new Archetype());
		new_archetype->_type = id;

		for (ArchetypeID::size_type i = 0; i < id.size(); ++i) // add empty array for each component in type
		{
			new_archetype->_component_data.push_back(ComponentData(new unsigned char[0])); // C++17 supports array delete
			new_archetype->_component_data_size.push_back(0);
		}

		return _archetypes.emplace_back(new_archetype).get();
	}

	template<class C>
	inline C* ECS::get_component(const EntityID entityId)
	{
		return nullptr;
	}

	template<class ...Ts>
	inline std::vector<EntityID> ECS::get_entities_with()
	{
		return std::vector<EntityID>();
	}
}