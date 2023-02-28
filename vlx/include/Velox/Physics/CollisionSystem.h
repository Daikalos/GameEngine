#pragma once

#include <vector>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Utilities.hpp>

#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>

#include "Shape.h"
#include "Circle.h"
#include "Box.h"
#include "Polygon.h"
#include "CollisionResult.h"
#include "CollisionData.h"
#include "CollisionTable.h"

namespace vlx
{
	class CollisionSystem final : public SystemAction
	{
	private:
		using Element = std::tuple<Shape*, LocalTransform*, Transform*>;
		using QTElement = QTElement<Element>;

		using CircleSystem = System<Circle, LocalTransform, Transform, QTElement>;
		using BoxSystem = System<Box, LocalTransform, Transform, QTElement>;

		struct CollisionPair
		{
			Shape* A{ nullptr };
			Shape* B{ nullptr };
		};

	public:
		CollisionSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	public:

	private:
		void BroadPhase();
		void NarrowPhase();

		void InsertShape(Shape& shape, LocalTransform& local_transform, Transform& transform, QTElement& element);

	private:
		CircleSystem	m_check_circles;
		BoxSystem		m_check_boxes;

		LQuadTree<Element> m_quad_tree;

		std::vector<CollisionPair>	m_collisions_pair;
		std::vector<CollisionData>	m_collisions_data;
	};
}