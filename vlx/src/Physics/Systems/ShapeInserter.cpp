#include <Velox/Physics/Systems/ShapeInserter.h>

#include <Velox/Physics/Systems/BroadSystem.h>
#include <Velox/Physics/CollisionBody.h>

using namespace vlx;

template<std::derived_from<Shape> S>
ShapeInserter<S>::ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system), m_insert(entity_admin, id)
{
	m_insert.Each(&ShapeInserter<S>::InsertShape, this);

	m_insert.SetPriority(2.0f);

	m_on_add_id = entity_admin.RegisterOnAddListener<S>(
		[this](EntityID eid, S& s)
		{
			m_broad.CreateBody(eid, &s, s.GetType());
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

template<std::derived_from<Shape> S>
ShapeInserter<S>::~ShapeInserter()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<S>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<S>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<S>(m_on_remove_id);
}

template<std::derived_from<Shape> S>
void ShapeInserter<S>::InsertShape(EntityID entity_id, S& shape, ColliderAABB& ab, QTBody& qtb)
{
	if (!qtb.GetEnabled())
		return;

	if (!qtb.Contains(ab.GetAABB()))
	{
		const auto it = m_broad.m_entity_body_map.find(entity_id);
		if (it == m_broad.m_entity_body_map.end())
			return;

		assert(it->second != BroadSystem::NULL_BODY);

		qtb.Erase();
		qtb.Insert(m_broad.m_quad_tree, ab.GetAABB().Inflate(P_AABB_INFLATE), it->second);
	}
}

ShapeInserter<Point>::ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) 
	: m_broad(broad_system)
{
	m_on_add_id = entity_admin.RegisterOnAddListener<Point>(
		[this](EntityID eid, Point& p)
		{
			m_broad.CreateBody(eid, &p, p.GetType());
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<Point>(
		[this](EntityID eid, Point& p)
		{
			if (auto i = m_broad.FindBody(eid); i != BroadSystem::NULL_BODY)
				m_broad.m_bodies[i].shape = &p;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<Point>(
		[this](EntityID eid, Point& p)
		{
			m_broad.RemoveBody(eid);
		});
}

ShapeInserter<Point>::~ShapeInserter()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<Point>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<Point>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<Point>(m_on_remove_id);
}

// explicit template instantiations

template class vlx::ShapeInserter<Circle>;
template class vlx::ShapeInserter<Box>;
template class vlx::ShapeInserter<Polygon>;