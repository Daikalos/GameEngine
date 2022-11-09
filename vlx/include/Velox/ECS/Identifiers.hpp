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

	constexpr EntityID NULL_ENTITY			= NULL;
	constexpr ArchetypeID NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE = 0,
		LYR_OBJECTS = 97,
		LYR_TRANSFORM = 98,
		LYR_ANCHOR = 99,
		LYR_RENDERING = 100
	};
}