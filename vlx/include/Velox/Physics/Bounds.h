#pragma once

namespace vlx
{
	struct Circle;
	struct Rectangle;
	struct CollisionResult;

	struct Bounds
	{
		virtual CollisionResult Collide(const Circle& other) = 0;
		virtual CollisionResult Collide(const Rectangle& other) = 0;
	};
}