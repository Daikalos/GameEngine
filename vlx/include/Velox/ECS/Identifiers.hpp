#pragma once

#include <cstdint>
#include <vector>

#include <Velox/Algorithms/SmallVector.hpp>

#include <Velox/Utilities/Concepts.h>

namespace vlx
{
	using IDType			= std::uint32_t;
	using LayerType			= std::uint16_t;
	using ColumnType		= std::uint16_t;

	using ByteArray			= std::byte[];
	using DataPtr			= std::byte*;

	using EntityID			= IDType;
	using ComponentTypeID	= std::size_t;
	using ArchetypeID		= std::size_t;
	using ComponentIDs		= SmallVector<ComponentTypeID>;

	template<class... Cs> requires IsComponents<Cs...>
	using ArrComponentIDs	= std::array<ComponentTypeID, sizeof...(Cs)>;

	static constexpr EntityID			NULL_ENTITY		= NULL;
	static constexpr ComponentTypeID	NULL_COMPONENT	= NULL;
	static constexpr ArchetypeID		NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE		= 0,
		LYR_GUI			= 30000,
		LYR_OBJECTS		= 35000,
		LYR_TRANSFORM	= 45000,
		LYR_ANCHOR		= 50000,
		LYR_PHYSICS		= 55000,
		LYR_CULLING		= 59999,
		LYR_RENDERING	= 60000
	};
}