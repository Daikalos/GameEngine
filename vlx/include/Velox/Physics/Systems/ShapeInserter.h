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
	class VELOX_API ShapeInserter
	{
	public:
		using ShapeSystem = System<S, Collider>;
		using ShapeBodySystem = System<S, Collider, PhysicsBody>;

	public:
		ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~ShapeInserter();

	private:
		void InsertShape(EntityID entity_id, Shape* shape, Collider* c);

	private:
		BroadSystem&	m_broad;
		ShapeSystem		m_insert;

		int				m_on_add_id		{-1};
		int				m_on_move_id	{-1};
		int				m_on_remove_id	{-1};
	};

	template<>
	class VELOX_API ShapeInserter<Point> // specialize for point since insertion is not required
	{
	public:
		using ShapeSystem = System<Point, Collider>;
		using ShapeBodySystem = System<Point, Collider, PhysicsBody>;

	public:
		ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~ShapeInserter();

	private:
		BroadSystem&	m_broad;

		int				m_on_add_id		{-1};
		int				m_on_move_id	{-1};
		int				m_on_remove_id	{-1};
	};
}