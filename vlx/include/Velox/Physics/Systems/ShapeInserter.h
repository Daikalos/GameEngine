#pragma once

#include <Velox/ECS/System.hpp>

#include <Velox/System/EventID.h>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Point.h>

#include "../Collider/Collider.h"
#include "../Collider/ColliderAABB.h"

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

	private:
		void InsertShape(EntityID entity_id, S& shape, ColliderAABB& ab, QTBody& qtb);

	private:
		BroadSystem&	m_broad;
		InsertSystem	m_insert;

		EventID	m_on_add_id;
		EventID	m_on_move_id;
		EventID	m_on_remove_id;
	};

	template<>
	class VELOX_API ShapeInserter<Point> // specialize for point since insertion is not required
	{
	public:
		ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system);

	private:
		BroadSystem&	m_broad;

		EventID			m_on_add_id;
		EventID			m_on_move_id;
		EventID			m_on_remove_id;
	};
}