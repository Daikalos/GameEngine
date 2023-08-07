#pragma once

#include "LocalManifold.h"

namespace vlx
{
	class CollisionBody;

	class CollisionArbiter
	{
	public:
		CollisionBody* A {nullptr};
		CollisionBody* B {nullptr};

		LocalManifold manifold;
	};
}