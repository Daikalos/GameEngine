#pragma once

#include <cstdint>
#include <vector>
#include <span>

#include <Velox/Structures/SmallVector.hpp>
#include <Velox/System/Concepts.h>
#include <Velox/Types.hpp>

namespace vlx
{
	template<class C>
	concept IsComponent = std::is_class_v<C> && std::semiregular<std::remove_const_t<C>> && sizeof(C) >= 1 && sizeof(C) <= std::numeric_limits<uint32>::max();

	template<class... Cs>
	concept IsComponents = (IsComponent<Cs> && ...) && Exists<Cs...> && NoDuplicates<Cs...>;

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

	using ComponentIDSpan	= std::span<const ComponentTypeID>;

	using EntitySpan		= std::span<const EntityID>;

	inline constexpr EntityID			NULL_ENTITY		= NULL;
	inline constexpr ComponentTypeID	NULL_COMPONENT	= NULL;
	inline constexpr ArchetypeID		NULL_ARCHETYPE	= NULL;

	enum SystemLayers : LayerType
	{
		LYR_NONE				= 0,
		LYR_GUI					= 30000,

		LYR_OBJECTS_START		= 33000,
		LYR_OBJECTS_PRE			= 34000,
		LYR_OBJECTS_UPDATE		= 35000,
		LYR_OBJECTS_FIXED		= 36000,
		LYR_OBJECTS_POST		= 37000,

		LYR_ANIMATION			= 38000,
		LYR_ANCHOR				= 40000,
		LYR_LOCAL_TRANSFORM		= 45000,
		LYR_GLOBAL_TRANSFORM	= 46000,

		LYR_DIRTY_PHYSICS		= 47500,
		LYR_BROAD_PHASE			= 50000,
		LYR_PHYSICS				= 52500,

		LYR_CULLING				= 59000,
		LYR_RENDERING			= 60000
	};
}