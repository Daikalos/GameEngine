#pragma once

#include <vector>

#include <Velox/Utilities.hpp>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../CollisionResult.h"
#include "../CollisionData.h"
#include "../CollisionTable.h"

namespace vlx
{
	class VELOX_API BroadSystem final : public SystemAction
	{
	public:
		using Element = std::tuple<Shape*, LocalTransform*, Transform*>;
		using QTElement = QTElement<Element>;

		using CircleSystem = System<Circle, LocalTransform, Transform, QTElement>;
		using BoxSystem = System<Box, LocalTransform, Transform, QTElement>;

		struct CollisionPair
		{
			Shape* A {nullptr};
			Shape* B {nullptr};
		};

	public:
		BroadSystem(EntityAdmin& entity_admin, const LayerType id);


	};
}