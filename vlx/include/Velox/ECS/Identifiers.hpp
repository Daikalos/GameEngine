#pragma once

#include <cstdint>
#include <vector>

#include <Velox/Algorithms/SmallVector.hpp>

#include <Velox/System/Concepts.h>

namespace vlx
{
	using IDType			= std::uint32_t;
	using LayerType			= std::uint32_t;
	using ColumnType		= std::uint16_t;

	using ByteArray			= std::byte[];
	using DataPtr			= std::byte*;

	using EntityID			= IDType;
	using ComponentTypeID	= std::size_t;
	using ArchetypeID		= std::size_t;
	using ComponentIDs		= SmallVector<ComponentTypeID, 16>;

	template<class... Cs> requires IsComponents<Cs...>
	using ArrComponentIDs	= std::array<ComponentTypeID, sizeof...(Cs)>;

	inline constexpr EntityID			NULL_ENTITY		= NULL;
	inline constexpr ComponentTypeID	NULL_COMPONENT	= NULL;
	inline constexpr ArchetypeID		NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE			= 0,
		LYR_GUI				= 300000,
		LYR_OBJECTS			= 350000,
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