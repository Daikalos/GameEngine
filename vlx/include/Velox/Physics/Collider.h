#pragma once

#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/System/Event.hpp>

#include "CollisionResult.h"
#include "CollisionLayer.h"
#include "CollisionObject.h"

namespace vlx
{
	using QTCollider = QTElement<CollisionObject>;

	class Collider : public QTCollider
	{
	public:
		Event<const CollisionResult&>	OnEnter;	// called when collider enters another collider
		Event<EntityID>					OnExit;		// called when collider exits another collider
		Event<const CollisionResult&>	OnOverlap;	// called when collider overlaps another collider

	public:
		CollisionLayer layer;
		bool enabled {true};

	private:
		bool dirty {true}; // if should update the AABB in the quadtree

		friend class PhysicsDirtySystem;
		friend class BroadSystem;
		friend class NarrowSystem;
		friend class PhysicsSystem;
	};
}