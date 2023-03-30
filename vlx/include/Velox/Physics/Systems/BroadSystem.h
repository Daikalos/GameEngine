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

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	public:
		using GeneralSystem			= System<Collision, LocalTransform>;
		using CircleSystem			= System<Circle, Collision, LocalTransform>;
		using BoxSystem				= System<Box, Collision, LocalTransform>;
		using CircleBodySystem		= System<Circle, Collision, PhysicsBody, LocalTransform>;
		using BoxBodySystem			= System<Box, Collision, PhysicsBody, LocalTransform>;

		using CollisionPair			= std::pair<CollisionObject, CollisionObject>;
		using CollisionList			= std::vector<CollisionPair>;

	private:
		using CollisionIndices		= std::vector<std::uint32_t>;

	public:
		BroadSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void Update();

	private:
		void InsertShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt);
		void QueryShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt);

		void CullDuplicates();

	private:
		EntityAdmin* m_entity_admin {nullptr};
		LayerType	 m_layer		{LYR_NONE};

		LQuadTree<QTCollision::value_type> m_quad_tree;

		CollisionList		m_collision_pairs;
		CollisionIndices	m_collision_indices;

		CircleSystem		m_circles_ins;
		BoxSystem			m_boxes_ins;
		CircleBodySystem	m_circles_body_ins;
		BoxBodySystem		m_boxes_body_ins;

		GeneralSystem		m_cleanup;

		CircleSystem		m_circles_query;
		BoxSystem			m_boxes_query;
		CircleBodySystem	m_circles_body_query;
		BoxBodySystem		m_boxes_body_query;

		friend class NarrowSystem;
	};
}