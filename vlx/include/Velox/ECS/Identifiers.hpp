#pragma once

#include <cstdint>
#include <vector>

namespace vlx
{
	using IDType			= typename std::uint32_t;
	using ByteArray			= typename unsigned char[];
	using DataPtr			= typename unsigned char*;

	using EntityID			= typename IDType;
	using ComponentTypeID	= typename IDType;
	using ArchetypeID		= typename std::vector<ComponentTypeID>;

	const IDType NULL_ENTITY = NULL;
}