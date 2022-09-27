#pragma once

#include <memory>
#include <vector>
#include <queue>

#include "Identifiers.hpp"

namespace vlx
{
	using ComponentData = typename std::unique_ptr<ByteArray>;

	struct Archetype // an archetype for every unique list of components for an entity
	{
		ArchetypeID					id{NULL_ARCHETYPE};
		ComponentIDs				type;				// all the ids of the components
		std::vector<EntityID>		entities;			// all the entities registered to this archetype
		std::vector<ComponentData>	component_data;		// all the components data in this archetype stored by type
		std::vector<std::size_t>	component_data_size;
		std::queue<std::size_t>		add_locations; // TODO: a new system to prevent invalidating pointers by only constructing new entities at points marked in this queue
	};
}