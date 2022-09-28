#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the relationship between entities that allows for scene graphs 
	/// and other things...
	/// </summary>
	struct Relationship
	{
		EntityID parent			{NULL_ENTITY};
		EntityID first			{NULL_ENTITY};
		EntityID prev			{NULL_ENTITY};
		EntityID next			{NULL_ENTITY};
		std::size_t children	{0};
	};
}