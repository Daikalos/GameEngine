#pragma once

namespace vlx
{
	struct Bounds;
	struct CollisionResult;

	struct Shape
	{
		virtual CollisionResult Collide(Bounds& bounds) const = 0;
	};
}