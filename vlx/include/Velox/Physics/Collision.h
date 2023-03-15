#pragma once

#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/ECS/IComponent.h>
#include <Velox/System/Event.hpp>

#include "CollisionResult.h"
#include "CollisionLayer.h"
#include "CollisionObject.h"

namespace vlx
{
	using QTCollision = QTElement<CollisionObject>;

	class Collision : public QTCollision
	{
	public:
		Event<CollisionResult> OnEnter;		// common events for collisions
		Event<CollisionResult> OnExit;
		Event<CollisionResult> OnOverlap;

	public:
		CollisionLayer	layer;
		bool			enabled		{true};

	private:
		bool colliding	{false};
		bool dirty		{true}; // if should update the AABB in the quadtree

		friend class BroadSystem;
		friend class NarrowSystem;
		friend class PhysicsSystem;
	};
}