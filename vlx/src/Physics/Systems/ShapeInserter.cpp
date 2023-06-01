#include <Velox/Physics/Systems/ShapeInserter.h>
#include <Velox/Physics/Systems/BroadSystem.h>

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

	m_on_add_id = entity_admin.RegisterOnAddListener<S>([this](EntityID eid, S& s)
		{
			int i = m_broad.TryAddNewObject(eid);
			m_broad.m_bodies[i].shape = &s;
			m_broad.m_bodies[i].type = s.GetType();
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<S>([this](EntityID eid, S& s)
		{
			const auto it = m_broad.m_entity_body_map.find(eid);
			if (it != m_broad.m_entity_body_map.end())
				m_broad.m_bodies[m_broad.m_bodies_ptr[it->second].element].shape = &s;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<S>([this](EntityID eid, S& s)
		{
			const auto it = m_broad.m_entity_body_map.find(eid);
			if (it != m_broad.m_entity_body_map.end())
			{
				m_broad.m_bodies[m_broad.m_bodies_ptr[it->second].element].shape = nullptr;
				if (m_broad.RemoveEmptyObject(it->second))
					m_broad.m_entity_body_map.erase(it);
			}
		});

	m_insert.SetPriority(2.0f);
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
			int i = m_broad.TryAddNewObject(eid);
			m_broad.m_bodies[i].shape = &p;
			m_broad.m_bodies[i].type = p.GetType();
		});

	m_on_move_id = entity_admin.RegisterOnMoveListener<Point>([this](EntityID eid, Point& p)
		{
			const auto it = m_broad.m_entity_body_map.find(eid);
			if (it != m_broad.m_entity_body_map.end())
				m_broad.m_bodies[it->second].shape = &p;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<Point>([this](EntityID eid, Point& p)
		{
			const auto it = m_broad.m_entity_body_map.find(eid);
			if (it != m_broad.m_entity_body_map.end())
			{
				m_broad.m_bodies[it->second].shape = nullptr;
				if (m_broad.RemoveEmptyObject(it->second))
					m_broad.m_entity_body_map.erase(it);
			}
		});
}

ShapeInserter<Point>::~ShapeInserter()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<Point>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<Point>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<Point>(m_on_remove_id);
}

template class ShapeInserter<Circle>;
template class ShapeInserter<Box>;