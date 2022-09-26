#pragma once

#include <cstdint>
#include <vector>

namespace vlx
{
	using IDType			= typename std::uint32_t;
	using ByteArray			= typename std::byte[];
	using DataPtr			= typename std::byte*;
	using LayerType			= typename std::uint16_t;

	using EntityID			= typename IDType;
	using ComponentTypeID	= typename IDType;
	using ArchetypeID		= typename IDType;
	using ComponentIDs		= typename std::vector<ComponentTypeID>;

	constexpr EntityID NULL_ENTITY = NULL;
	constexpr ArchetypeID NULL_ARCHETYPE = NULL;
}