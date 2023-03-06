#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/Utilities/Time.h>

namespace vlx
{
	struct CollisionResult
	{
		EntityID		A				{NULL_ENTITY};
		EntityID		B				{NULL_ENTITY};
		bool			collided		{false};
		float			penetration		{0.0f};
		Vector2f	normal;
	};
}