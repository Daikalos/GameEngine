#pragma once

#include <memory>

#include "Identifiers.hpp"

namespace vlx
{
	using ComponentData = std::unique_ptr<ByteArray>;


	struct Archetype // an archetype for every unique list of components for an entity
	{
		ArchetypeID					type;				// all the ids of the components
		std::vector<EntityID>		entities;			// all the entities registered to this archetype
		std::vector<ComponentData>	component_data;		// all the type of components in this archetype
		std::vector<std::size_t>	component_data_size;
	};
}