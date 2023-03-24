#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/System/Vector2.hpp>

namespace vlx
{
	struct CollisionResult
	{
		EntityID		A				{NULL_ENTITY};
		EntityID		B				{NULL_ENTITY};
		Vector2f		normal;
		Vector2f		hit;
		float			penetration		{0.0f};
	};
}