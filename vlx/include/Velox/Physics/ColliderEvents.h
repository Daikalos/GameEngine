#pragma once

#include <functional>

#include <Velox/ECS/Identifiers.hpp>

#include "CollisionResult.h"

namespace vlx
{
	/// Called when collider enters another collider
	/// 
	struct ColliderEnter
	{
		std::function<void(const CollisionResult&)> OnEnter;
	};

	/// Called when collider exits another collider
	/// 
	struct ColliderExit
	{
		std::function<void(EntityID)> OnExit;
	};

	/// Called when collider overlaps another collider
	/// 
	struct ColliderOverlap
	{
		std::function<void(const CollisionResult&)> OnOverlap;
	};
}