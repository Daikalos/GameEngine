#pragma once

#include <cstdint>
#include <vector>

namespace vlx
{
	using IDType			= typename std::uint32_t;
	using ByteArray			= typename std::byte[];
	using DataPtr			= typename std::byte*;

	using EntityID			= typename IDType;
	using ComponentTypeID	= typename IDType;
	using ArchetypeID		= typename std::vector<ComponentTypeID>;

	constexpr IDType NULL_ENTITY = NULL;
}