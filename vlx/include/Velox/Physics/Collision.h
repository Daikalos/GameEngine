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
		void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) override
		{
			// since the collision object stores pointers to components,
			// they would need to be updated if their data location is updated
			m_dirty = true;
		}

	public:
		Event<CollisionResult> OnEnter;		// common events for collisions
		Event<CollisionResult> OnExit;
		Event<CollisionResult> OnOverlap;

		CollisionLayer Layer;
		bool Enabled {true};
		
	private:
		bool m_dirty {true}; // if should update the AABB in the quadtree

		friend class BroadSystem;
		friend class NarrowSystem;
	};
}