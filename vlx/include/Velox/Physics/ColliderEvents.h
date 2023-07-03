#pragma once

#include <Velox/System/Event.hpp>
#include <Velox/ECS/Identifiers.hpp>

#include "CollisionResult.h"

namespace vlx
{
	/// Called when collider enters another collider
	/// 
	struct ColliderEnter
	{
		Event<const CollisionResult&> OnEnter;
	};

	/// Called when collider exits another collider
	/// 
	struct ColliderExit
	{
		Event<EntityID>	OnExit;
	};

	/// Called when collider overlaps another collider
	/// 
	struct ColliderOverlap
	{
		Event<const CollisionResult&> OnOverlap;
	};
}