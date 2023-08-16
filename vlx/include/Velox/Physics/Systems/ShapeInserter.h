#pragma once

#include <Velox/System/EventID.h>
#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class BroadSystem;
	class EntityAdmin;

	template<std::derived_from<Shape> S>
	class VELOX_API ShapeInserter
	{
	public:
		ShapeInserter(EntityAdmin& entity_admin, BroadSystem& broad_system);

	private:
		BroadSystem& m_broad;

		EventID	m_on_add_id;
		EventID	m_on_move_id;
		EventID	m_on_remove_id;
	};
}