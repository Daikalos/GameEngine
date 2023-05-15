#pragma once

#include <Velox/ECS/System.hpp>
#include <Velox/ECS/EntityAdmin.h>
#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Point.h>

#include "../CollisionObject.h"
#include "../Collider.h"

namespace vlx
{
	class BroadSystem;

	template<class S>
	class VELOX_API BroadShapeHelper
	{
	public:
		using ShapeSystem = System<S, Collider>;
		using ShapeBodySystem = System<S, Collider, PhysicsBody>;

	public:
		BroadShapeHelper(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~BroadShapeHelper();

	private:
		void InsertShape(EntityID entity_id, Shape* shape, Collider* c);
		void QueryShape(EntityID entity_id, Shape* shape, typename Shape::Type type, Collider* c, PhysicsBody* pb);

	private:
		BroadSystem&	m_broad;

		ShapeSystem		m_insert;
		ShapeSystem		m_query;
		ShapeBodySystem	m_body_query;

		int				m_on_add_id		{-1};
		int				m_on_move_id	{-1};
		int				m_on_remove_id	{-1};
	};

	template<>
	class VELOX_API BroadShapeHelper<Point> // specialize for point since insertion is not required
	{
	public:
		using ShapeSystem = System<Point, Collider>;
		using ShapeBodySystem = System<Point, Collider, PhysicsBody>;

	public:
		BroadShapeHelper(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~BroadShapeHelper();

	private:
		void QueryPoint(EntityID entity_id, Point* point, Collider* c, PhysicsBody* pb);

	private:
		BroadSystem& m_broad;

		ShapeSystem		m_query;
		ShapeBodySystem m_body_query;

		int				m_on_add_id		{-1};
		int				m_on_move_id	{-1};
		int				m_on_remove_id	{-1};
	};
}