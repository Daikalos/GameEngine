#pragma once

#include <memory>

#include "Identifiers.h"

namespace fge
{
	using ComponentData = std::shared_ptr<unsigned char>;

	struct Archetype // an archetype for every unique list of components for an entity
	{
		ArchetypeID _type;								// all the ids of the components
		std::vector<ComponentData> _component_data;		// all the type of components in this archetype
		std::vector<EntityID> _entitity_ids;			// all the entities registered to this archetype
		std::vector<std::size_t> _component_data_size;
	};
}