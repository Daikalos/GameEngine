#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/System/Vector2.hpp>

namespace vlx
{
	struct CollisionResult
	{
		EntityID		A				{NULL_ENTITY};
		EntityID		B				{NULL_ENTITY};
		bool			collided		{false};
		float			penetration		{0.0f};
		Vector2f		normal;
	};
}