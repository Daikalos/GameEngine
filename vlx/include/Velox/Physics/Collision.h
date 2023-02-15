#pragma once

namespace vlx
{
	struct Circle;
	struct Rectangle;
	struct CollisionResult;

	struct Collision
	{
		friend static CollisionResult Collide(Circle& c1, Circle& c2);
		friend static CollisionResult Collide(Rectangle& r1, Rectangle& r2);
		friend static CollisionResult Collide(Circle& circle, Rectangle& rectangle);
	};
}