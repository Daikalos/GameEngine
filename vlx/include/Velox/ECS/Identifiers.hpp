#pragma once

#include <cstdint>
#include <vector>

#include <Velox/Algorithms/SmallVector.hpp>
#include <Velox/System/Concepts.h>
#include <Velox/VeloxTypes.hpp>

namespace vlx
{
	using IDType			= uint32;
	using LayerType			= uint32;
	using ColumnType		= uint16;

	using ByteArray			= std::byte[];
	using DataPtr			= std::byte*;

	using EntityID			= IDType;
	using ComponentTypeID	= uint64;
	using ArchetypeID		= uint64;
	using ComponentIDs		= std::vector<ComponentTypeID>;

	template<class... Cs> requires IsComponents<Cs...>
	using ArrComponentIDs	= std::array<ComponentTypeID, sizeof...(Cs)>;

	inline constexpr EntityID			NULL_ENTITY		= NULL;
	inline constexpr ComponentTypeID	NULL_COMPONENT	= NULL;
	inline constexpr ArchetypeID		NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE			= 0,
		LYR_GUI				= 300000,
		LYR_OBJECTS_START	= 330000,
		LYR_OBJECTS_PRE		= 340000,
		LYR_OBJECTS_UPDATE	= 350000,
		LYR_OBJECTS_FIXED	= 360000,
		LYR_OBJECTS_POST	= 370000,
		LYR_ANIMATION		= 380000,
		LYR_ANCHOR			= 400000,
		LYR_TRANSFORM		= 450000,
		LYR_DIRTY_PHYSICS	= 475000,
		LYR_BROAD_PHASE		= 500000,
		LYR_NARROW_PHASE	= 525000,
		LYR_PHYSICS			= 550000,
		LYR_CULLING			= 599999,
		LYR_RENDERING		= 600000
	};
}