#pragma once

#include <vector>

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

#include "../CollisionObject.h"
#include "../CollisionResult.h"
#include "../CollisionData.h"
#include "../CollisionTable.h"
#include "../Collision.h"

namespace vlx
{
	class VELOX_API BroadSystem final : public SystemAction
	{
	public:
		using DirtyGlobalSystem		= System<Collision, Transform>;
		using DirtyLocalSystem		= System<Collision, LocalTransform>;
		using CircleSystem			= System<Circle, Collision, LocalTransform, Transform>;
		using BoxSystem				= System<Box, Collision, LocalTransform, Transform>;
		using CollisionSystem		= System<Collision, LocalTransform, Transform>;

		struct CollisionPair
		{
			CollisionObject A;
			CollisionObject B;
		};

	public:
		BroadSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		NODISC constexpr bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void InsertShape(Shape& shape, Collision& c, LocalTransform& lt, Transform& t);

	private:
		LQuadTree<QTCollision::Value> m_quad_tree;
		std::vector<CollisionPair> m_collisions;

		DirtyGlobalSystem	m_dirty_transform;
		DirtyLocalSystem	m_dirty_physics;
		CircleSystem		m_circles;
		BoxSystem			m_boxes;
		CollisionSystem		m_broad_collisions;
	};
}