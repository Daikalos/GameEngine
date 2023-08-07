#pragma once

#include "Physics/Shapes/Shape.h"
#include "Physics/Shapes/Box.h"
#include "Physics/Shapes/Circle.h"
#include "Physics/Shapes/Point.h"
#include "Physics/Shapes/Polygon.h"

#include "Physics/Collider/Collider.h"
#include "Physics/Collider/ColliderAABB.h"
#include "Physics/Collider/ColliderEvents.h"

#include "Physics/Collision/CollisionLayer.h"
#include "Physics/Collision/CollisionResult.h"
#include "Physics/Collision/CollisionTable.h"
#include "Physics/Collision/CollisionArbiter.h"
#include "Physics/Collision/CollisionBody.h"

#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsCommon.hpp"
#include "Physics/PhysicsMaterial.h"

#include "Physics/Systems/PhysicsDirtySystem.h"
#include "Physics/Systems/BroadSystem.h"
#include "Physics/Systems/NarrowSystem.h"
#include "Physics/Systems/PhysicsSystem.h"