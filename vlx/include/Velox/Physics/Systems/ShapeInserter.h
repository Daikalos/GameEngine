#pragma once

#include <Velox/ECS/System.hpp>
#include <Velox/Physics/Shapes/Shape.h>

#include "../Collider.h"

namespace vlx
{
	class BroadSystem;
	class Point;

	template<class S>
	class VELOX_API ShapeInserter
	{
	public:
		using ShapeSystem = System<S, Collider>;

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
		ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~ShapeInserter();

	private:
		BroadSystem&	m_broad;

		int				m_on_add_id		{-1};
		int				m_on_move_id	{-1};
		int				m_on_remove_id	{-1};
	};
}