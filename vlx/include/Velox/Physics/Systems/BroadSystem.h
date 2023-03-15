#pragma once

#include <vector>
#include <unordered_map>

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

#include "PhysicsSystem.h"

namespace vlx
{
	class VELOX_API BroadSystem final : public SystemAction
	{
	public:
		using DirtyGlobalSystem		= System<Collision, Transform>;
		using DirtyLocalSystem		= System<Collision, LocalTransform>;
		using GeneralSystem			= System<Collision, LocalTransform, Transform>;
		using CircleSystem			= System<Circle, Collision, LocalTransform, Transform>;
		using BoxSystem				= System<Box, Collision, LocalTransform, Transform>;
		using CircleBodySystem		= System<Circle, Collision, PhysicsBody, LocalTransform, Transform>;
		using BoxBodySystem			= System<Box, Collision, PhysicsBody, LocalTransform, Transform>;

		using CollisionPair			= std::pair<CollisionObject, CollisionObject>;

	public:
		BroadSystem(EntityAdmin& entity_admin, const LayerType id, PhysicsSystem& physics_system);

	public:
		NODISC constexpr bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void InsertShape(const EntityID entity_id, Shape* shape, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t);
		void QueryShape(Shape* shape, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t);

		void CullDuplicates();

	private:
		PhysicsSystem* m_physics_system {nullptr};

		LQuadTree<QTCollision::value_type> m_quad_tree;

		std::vector<CollisionPair> m_collision_pairs;
		std::vector<std::uint32_t> m_collision_indices;
		std::vector<std::uint32_t> m_unique_collisions;

		DirtyGlobalSystem	m_dirty_transform;
		DirtyLocalSystem	m_dirty_physics;

		CircleSystem		m_circles_ins;
		BoxSystem			m_boxes_ins;
		CircleBodySystem	m_circles_body_ins;
		BoxBodySystem		m_boxes_body_ins;

		GeneralSystem		m_cleanup;

		CircleSystem		m_circles_query;
		BoxSystem			m_boxes_query;
		CircleBodySystem	m_circles_body_query;
		BoxBodySystem		m_boxes_body_query;
	};
}