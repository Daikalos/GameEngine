#pragma once

#include <cstdint>
#include <vector>

namespace vlx
{
	using IDType			= typename std::uint32_t;
	using ByteArray			= typename std::uint8_t[];
	using DataPtr			= typename std::uint8_t*;

	using EntityID			= typename IDType;
	using ComponentTypeID	= typename IDType;
	using ArchetypeID		= typename std::vector<ComponentTypeID>;

	constexpr IDType NULL_ENTITY = NULL;
}