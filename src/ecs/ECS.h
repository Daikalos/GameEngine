#pragma once

#include <unordered_map>
#include <vector>

#include "Identifiers.h"
#include "Archetype.h"
#include "Component.h"

namespace fge
{
	class SystemBase;

	class ECS
	{
	private:
		struct Record
		{
			Archetype* archetype;
			std::size_t index; // where in the ArchetypesArray is the entity located in
		};

		using CompnentTypeIDBaseMap = std::unordered_map<ComponentTypeID, ComponentBase*>;
		using EntityArchetypeMap = std::unordered_map<EntityID, Record>;				   
		using ArchetypesArray = std::vector<Archetype*>;								   // find matching archetype to update matching entities
		using SystemsArrayMap = std::unordered_map<std::uint8_t, std::vector<SystemBase*>>; // map layer to array of systems (layer allows for controlling the order of calls)

	public:
		ECS();
		~ECS();

		EntityID get_new_id();

		template<class T>
		void register_component();

		template<class T>
		bool is_component_registered();

	private:
		EntityArchetypeMap _entity_archetype_map;
	};
}