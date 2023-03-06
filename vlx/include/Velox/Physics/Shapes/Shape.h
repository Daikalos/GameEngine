#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/Graphics/Rectangle.hpp>
#include <Velox/Utilities/Event.hpp>
#include <Velox/Config.hpp>

#include "../CollisionResult.h"
#include "../PhysicsBody.h"

namespace vlx
{
	struct VELOX_API Shape : public IComponent
	{
		enum Type : short
		{
			Circle,
			Box,
			Point,
			Convex,

			// Not sure if the engine will support concave shapes, seems very difficult to implement and may cost a lot of performance and memory,
			// http://wscg.zcu.cz/wscg2004/Papers_2004_Full/B83.pdf is the closest that presents a viable solution

			Count // always keep last
		};

		virtual ~Shape() = 0;

		virtual void Initialize(PhysicsBody& body) const = 0;
		virtual RectFloat GetAABB() const = 0;

		virtual constexpr Type GetType() const noexcept = 0;

		Event<CollisionResult> OnEnter;		// common events for collisions
		Event<CollisionResult> OnExit;
		Event<CollisionResult> OnOverlap;
	};

	inline Shape::~Shape() = default;
}