#include <Velox/Physics/Systems/ShapeInserter.h>

#include <Velox/Physics/Systems/BroadSystem.h>
#include <Velox/Physics/CollisionBody.h>

using namespace vlx;

template<class S>
ShapeInserter<S>::ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system), m_insert(entity_admin, id)
{
	m_insert.Each(
		[this](EntityID entity_id, S& s, Collider& c)
		{
			InsertShape(entity_id, &s, &c);
		});

	m_insert.SetPriority(2.0f);

	m_on_add_id = entity_admin.RegisterOnAddListener<S>([this](EntityID eid, S& s)
		{
			m_broad.CreateBody(eid, &s, s.GetType());
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<S>([this](EntityID eid, S& s)
		{
			if (auto i = m_broad.FindBody(eid); i != BroadSystem::NULL_BODY)
				m_broad.m_bodies[i].shape = &s;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<S>([this](EntityID eid, S& s)
		{
			m_broad.RemoveBody(eid);
		});
}

template<class S>
ShapeInserter<S>::~ShapeInserter()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<S>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<S>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<S>(m_on_remove_id);
}

template<class S>
void ShapeInserter<S>::InsertShape(EntityID entity_id, Shape* s, Collider* c)
{
	if (!c->GetEnabled())
		return;

	const RectFloat& shape_aabb = s->GetAABB();
	if (!c->Contains(shape_aabb))
	{
		const auto it = m_broad.m_entity_body_map.find(entity_id);
		if (it == m_broad.m_entity_body_map.end())
			return;

		c->Erase();
		c->Insert(m_broad.m_quad_tree, shape_aabb.Inflate(P_AABB_INFLATE), it->second);
	}
}

ShapeInserter<Point>::ShapeInserter(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) 
	: m_broad(broad_system)
{
	m_on_add_id = entity_admin.RegisterOnAddListener<Point>([this](EntityID eid, Point& p)
		{
			m_broad.CreateBody(eid, &p, p.GetType());
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<Point>([this](EntityID eid, Point& p)
		{
			if (auto i = m_broad.FindBody(eid); i != BroadSystem::NULL_BODY)
				m_broad.m_bodies[i].shape = &p;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<Point>([this](EntityID eid, Point& p)
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