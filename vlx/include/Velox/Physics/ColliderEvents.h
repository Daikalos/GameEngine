#pragma once

#include <Velox/System/Event.hpp>
#include <Velox/ECS/Identifiers.hpp>

#include "CollisionResult.h"

namespace vlx
{
	struct ColliderEvents
	{
		Event<const CollisionResult&>	OnEnter;	// called when collider enters another collider
		Event<EntityID>					OnExit;		// called when collider exits another collider
		Event<const CollisionResult&>	OnOverlap;	// called when collider overlaps another collider
	};
}