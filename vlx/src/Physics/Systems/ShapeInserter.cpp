#include <Velox/Physics/Systems/ShapeInserter.h>

#include <Velox/ECS/EntityAdmin.h>
#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

template<std::derived_from<Shape> S>
ShapeInserter<S>::ShapeInserter(EntityAdmin& entity_admin, BroadSystem& broad_system) :
	m_broad(broad_system)
{
	m_on_add_id = entity_admin.RegisterOnAddListener<S>(
		[this](EntityID eid, S& s)
		{
			m_broad.CreateBody(eid, &s, S::GetType());
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<S>(
		[this](EntityID eid, S& s)
		{
			if (auto i = m_broad.FindBody(eid); i != BroadSystem::NULL_BODY)
				m_broad.m_bodies[i].shape = &s;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<S>(
		[this](EntityID eid, S& s)
		{
			m_broad.RemoveBody(eid);
		});
}

// explicit template instantiations

template class ShapeInserter<Circle>;
template class ShapeInserter<Box>;
template class ShapeInserter<Polygon>;
template class ShapeInserter<Point>;