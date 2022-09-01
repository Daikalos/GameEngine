#pragma once

#include <memory>

#include "Identifiers.hpp"

namespace vlx
{
	using ComponentData = std::unique_ptr<unsigned char[]>;

	struct Archetype // an archetype for every unique list of components for an entity
	{
		ArchetypeID					m_type;					// all the ids of the components
		std::vector<ComponentData>	m_component_data;		// all the type of components in this archetype
		std::vector<EntityID>		m_entity_ids;			// all the entities registered to this archetype
		std::vector<std::size_t>	m_component_data_size;
	};
}