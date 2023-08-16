#pragma once

namespace vlx
{
	class CollisionBody;

	class CollisionArbiter
	{
	public:
		CollisionBody* A;
		CollisionBody* B;
	};
}