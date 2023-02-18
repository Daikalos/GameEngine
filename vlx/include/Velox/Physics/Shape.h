#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>

#include "CollisionResult.h"
#include "PhysicsBody.h"

namespace vlx
{
	struct Shape : public IComponent
	{
		enum Type : short
		{
			Circle,
			AABB,
			Convex,

			// not sure if engine will support concave shapes, seems very difficult to implement and may cost a lot of performance and memory,
			// http://wscg.zcu.cz/wscg2004/Papers_2004_Full/B83.pdf is the closest that presents a viable solution

			Count // always keep last
		};

		virtual constexpr Type GetType() const noexcept = 0;

		Event<CollisionResult> OnEnter;		// common events for collisions
		Event<CollisionResult> OnExit;
		Event<CollisionResult> OnOverlap;
	};
}