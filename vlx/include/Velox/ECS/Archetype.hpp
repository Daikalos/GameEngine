#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <Velox/Structures/SmallVector.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	using ComponentData = std::unique_ptr<ByteArray>;

	class Archetype;

	struct ArchetypeEdge
	{
		Archetype* add;
		Archetype* rmv;
	};

	class Archetype // an archetype for every unique list of components for an entity
	{
	private:
		using EdgesMap = std::unordered_map<ComponentTypeID, ArchetypeEdge>;

	public:
		ArchetypeID					id {NULL_ARCHETYPE};
		ComponentIDs				type;					// all the component ids
		std::vector<EntityID>		entities;				// all the entities registered to this archetype
		std::vector<ComponentData>	component_data;			// all the components data in this archetype stored by type
		std::vector<uint64>			component_data_size;	// total collective size in bytes of all components stored by type

		EdgesMap edges; // what set of component ids leads to which neighbouring archetype
	};
}