#pragma once

#include <cstdint>
#include <vector>

namespace vlx
{
	using IDType			= std::uint32_t;
	using LayerType			= std::uint16_t;
	using ColumnType		= std::uint16_t;

	using ByteArray			= std::byte[];
	using DataPtr			= std::byte*;

	using EntityID			= IDType;
	using ComponentTypeID	= IDType;
	using ArchetypeID		= IDType;
	using ComponentIDs		= std::vector<ComponentTypeID>;

	static constexpr EntityID NULL_ENTITY		= NULL;
	static constexpr ArchetypeID NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE		= 0,
		LYR_GUI			= 990,
		LYR_OBJECTS		= 991,
		LYR_TRANSFORM	= 992,
		LYR_ANCHOR		= 1000,
		LYR_RENDERING	= 1000
	};
}