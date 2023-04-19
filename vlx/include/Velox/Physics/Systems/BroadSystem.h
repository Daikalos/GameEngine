#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/ECS/System.hpp>
#include <Velox/ECS/EntityAdmin.h>
#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>
#include <Velox/Config.hpp>
#include <Velox/VeloxTypes.hpp>

#include "../CollisionObject.h"
#include "../Collider.h"

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	public:
		using GeneralSystem			= System<Collider, Transform>;

		using CollisionPair			= std::pair<CollisionObject, CollisionObject>;
		using CollisionIndex		= uint32;

		using CollisionList			= std::vector<CollisionPair>;
		using CollisionIndices		= std::vector<CollisionIndex>;

		using QuadTree				= LQuadTree<QTCollider::value_type>;

	private:
		template<class S>
		class ShapeQTBehaviour
		{
		public:
			using ShapeSystem = System<S, Collider>;
			using ShapeBodySystem = System<S, Collider, PhysicsBody>;

		public:
			ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system);

		private:
			void InsertShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collider* c, PhysicsBody* pb);
			void QueryShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collider* c, PhysicsBody* pb);

		private:
			BroadSystem&		m_broad;

			ShapeSystem		m_insert;
			ShapeBodySystem	m_body_insert;
			ShapeSystem		m_query;
			ShapeBodySystem	m_body_query;
		};

		template<>
		class VELOX_API ShapeQTBehaviour<Point> // specialize for point since insertion is not required
		{
		public:
			using ShapeSystem = System<Point, Collider>;
			using ShapeBodySystem = System<Point, Collider, PhysicsBody>;

		public:
			ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system);

		private:
			void QueryPoint(EntityID entity_id, Point* point, Collider* c, PhysicsBody* pb);

		private:
			BroadSystem&	m_broad;

			ShapeSystem		m_query;
			ShapeBodySystem m_body_query;
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
		void CullDuplicates();

	private:
		EntityAdmin*				m_entity_admin	{nullptr};
		LayerType					m_layer			{LYR_NONE};

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