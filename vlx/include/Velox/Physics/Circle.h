#pragma once

#include "Shape.h"
#include "Bounds.h"
#include "CollisionResult.h"

namespace vlx
{
	struct Circle : public Shape
	{
		float radius;

		CollisionResult Collide(Bounds& bounds) const override;
	};
}