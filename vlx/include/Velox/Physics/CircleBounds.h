#pragma once

#include "Bounds.h"
#include "CollisionResult.h"

namespace vlx
{
	struct CircleBounds : public Bounds
	{
		virtual CollisionResult Collide(const Circle& other) override;
		virtual CollisionResult Collide(const Rectangle& other) override;
	};
}