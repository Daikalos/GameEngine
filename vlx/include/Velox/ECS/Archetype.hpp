#pragma once

#include <memory>
#include <vector>

#include "Identifiers.hpp"

namespace vlx
{
	using ComponentData = typename std::unique_ptr<ByteArray>;

	// TODO: maybe switch to using arrays...

	struct Archetype // an archetype for every unique list of components for an entity
	{
		ArchetypeID					id{NULL_ARCHETYPE};
		ComponentIDs				type;					// all the component ids
		std::vector<EntityID>		entities;				// all the entities registered to this archetype
		std::vector<ComponentData>	component_data;			// all the components data in this archetype stored by type
		std::vector<std::size_t>	component_data_size;	// total collective size in bytes of all components stored by type
	};
}