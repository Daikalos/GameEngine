#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/ECS/System.hpp>
#include <Velox/ECS/EntityAdmin.h>
#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include "../CollisionObject.h"
#include "../Collision.h"

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	public:
		using GeneralSystem			= System<Collision, LocalTransform>;
		using CircleSystem			= System<Circle, Collision, LocalTransform>;
		using BoxSystem				= System<Box, Collision, LocalTransform>;
		using PointSystem			= System<Point, Collision, LocalTransform>;
		using CircleBodySystem		= System<Circle, Collision, PhysicsBody, LocalTransform>;
		using BoxBodySystem			= System<Box, Collision, PhysicsBody, LocalTransform>;
		using PointBodySystem		= System<Point, Collision, PhysicsBody, LocalTransform>;

		using CollisionPair			= std::pair<CollisionObject, CollisionObject>;
		using CollisionIndex		= std::uint32_t;

		using CollisionList			= std::vector<CollisionPair>;
		using CollisionIndices		= std::vector<CollisionIndex>;

		using QuadTree				= LQuadTree<QTCollision::value_type>;

	private:
		template<class S>
		class ShapeQTBehaviour
		{
		public:
			using ShapeSystem = System<S, Collision, LocalTransform>;
			using ShapeBodySystem = System<S, Collision, PhysicsBody, LocalTransform>;

		public:
			ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system);

		private:
			ShapeSystem		m_insertion;
			ShapeBodySystem m_body_insertion;
			ShapeSystem		m_queries;
			ShapeBodySystem m_body_queries;
		};

		template<>
		class VELOX_API ShapeQTBehaviour<Point> // specialize for point since insertion is not required
		{
		public:
			using ShapeSystem = System<Point, Collision, LocalTransform>;
			using ShapeBodySystem = System<Point, Collision, PhysicsBody, LocalTransform>;

		public:
			ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system);

		private:
			ShapeSystem		m_queries;
			ShapeBodySystem m_body_queries;
		};

	public:
		BroadSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void Update();

	public:
		auto GetPairs() noexcept -> std::span<CollisionPair>;
		auto GetIndices() noexcept -> std::span<CollisionIndex>;

		auto GetPairs() const noexcept -> std::span<const CollisionPair>;
		auto GetIndices() const noexcept -> std::span<const CollisionIndex>;

	private:
		void InsertShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt);
		void QueryShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt);

		void CullDuplicates();

	private:
		EntityAdmin*				m_entity_admin {nullptr};
		LayerType					m_layer		{LYR_NONE};

		QuadTree					m_quad_tree;
		
		CollisionList				m_pairs;
		CollisionIndices			m_indices;

		ShapeQTBehaviour<Circle>	m_circles;
		ShapeQTBehaviour<Box>		m_boxes;
		ShapeQTBehaviour<Point>		m_points;

		GeneralSystem				m_cleanup;

		template<class S>
		friend class ShapeQTNode;
	};
}