#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Time.h>
#include <Velox/Types.hpp>

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