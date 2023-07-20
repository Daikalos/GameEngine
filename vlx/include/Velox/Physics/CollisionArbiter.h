#pragma once

#include "CollisionObject.h"
#include "LocalManifold.h"

namespace vlx
{
	class CollisionArbiter
	{
	public:
		CollisionObject* A {nullptr};
		CollisionObject* B {nullptr};

		LocalManifold	 manifold;
	};
}