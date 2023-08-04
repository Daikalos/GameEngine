#pragma once

#include <Velox/ECS/System.hpp>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Point.h>

#include "../Collider.h"
#include "../ColliderAABB.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class BroadSystem;

	template<std::derived_from<Shape> S>
	class VELOX_API ShapeInserter
	{
	public:
		using InsertSystem = System<S, ColliderAABB, QTBody>;

	public:
		ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);
		~ShapeInserter();

	private:
		void InsertShape(EntityID entity_id, S& shape, ColliderAABB& ab, QTBody& qtb);

	private:
		BroadSystem&	m_broad;
		InsertSystem	m_insert;

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