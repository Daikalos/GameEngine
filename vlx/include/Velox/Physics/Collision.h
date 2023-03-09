#pragma once

#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/ECS/IComponent.h>
#include <Velox/System/Event.hpp>

#include "CollisionResult.h"
#include "CollisionLayer.h"

namespace vlx
{
	class Collision;

	using QTCollision = QTElement<std::tuple<Shape*, Collision*, LocalTransform*, Transform*>>;

	class Collision : public QTCollision
	{
	public:
		Event<CollisionResult> OnEnter;		// common events for collisions
		Event<CollisionResult> OnExit;
		Event<CollisionResult> OnOverlap;

		CollisionLayer Layer;
		bool Enabled {true};
		
	private:
		bool m_dirty {true};

		friend class BroadSystem;
	};
}