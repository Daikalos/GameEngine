#include <Velox/Physics/Systems/BroadShapeHelper.h>
#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

template<class S>
BroadShapeHelper<S>::BroadShapeHelper(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system), m_insert(entity_admin, id), m_query(entity_admin, id), m_body_query(entity_admin, id)
{
	m_insert.Each(
		[this](EntityID entity_id, S& s, Collider& c)
		{
			InsertShape(entity_id, &s, &c);
		});

	m_query.Each(
		[this](EntityID entity_id, S& s, Collider& c)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, nullptr);
		});

	m_body_query.Each(
		[this](EntityID entity_id, S& s, Collider& c, PhysicsBody& pb)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, &pb);
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
				m_broad.m_bodies[it->second].shape = &s;
		});

	m_on_remove_id = entity_admin.RegisterOnRemoveListener<S>([this](EntityID eid, S& s)
		{
			const auto it = m_broad.m_entity_body_map.find(eid);
			if (it != m_broad.m_entity_body_map.end())
			{
				m_broad.m_bodies[it->second].shape = nullptr;
				if (m_broad.RemoveEmptyObject(it->second))
					m_broad.m_entity_body_map.erase(it);
			}
		});

	m_query.Exclude<PhysicsBody>();

	m_insert.SetPriority(2.0f);
}

template<class S>
BroadShapeHelper<S>::~BroadShapeHelper()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<S>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<S>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<S>(m_on_remove_id);
}

template<class S>
void BroadShapeHelper<S>::InsertShape(EntityID entity_id, Shape* s, Collider* c)
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

template<class S>
void BroadShapeHelper<S>::QueryShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collider* c, PhysicsBody* pb)
{
	if (!c->GetEnabled())
		return;

	auto collisions = m_broad.m_quad_tree.Query(s->GetAABB());

	for (const auto& elt : collisions)
	{
		const auto& obj = m_broad.m_bodies[elt.item];
		
		if (entity_id == obj.entity_id) // no collision against self
			continue;

		if ((c->layer & obj.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && obj.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = obj.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active) // dont bother colliding if both are either disabled or sleeping
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, type, s, c, pb), obj);
		m_broad.m_indices.emplace_back(static_cast<uint32_t>(m_broad.m_pairs.size() - 1));
	}
}

BroadShapeHelper<Point>::BroadShapeHelper(EntityAdmin& entity_admin, LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system), m_query(entity_admin, id), m_body_query(entity_admin, id)
{
	m_query.Each(
		[this](EntityID entity_id, Point& p, Collider& c)
		{
			QueryPoint(entity_id, &p, &c, nullptr);
		});

	m_body_query.Each(
		[this](EntityID entity_id, Point& p, Collider& c, PhysicsBody& pb)
		{
			QueryPoint(entity_id, &p, &c, &pb);
		});

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

	m_query.Exclude<PhysicsBody>();
}

BroadShapeHelper<Point>::~BroadShapeHelper()
{
	m_broad.m_entity_admin->DeregisterOnAddListener<Point>(m_on_add_id);
	m_broad.m_entity_admin->DeregisterOnMoveListener<Point>(m_on_move_id);
	m_broad.m_entity_admin->DeregisterOnRemoveListener<Point>(m_on_remove_id);
}

void BroadShapeHelper<Point>::QueryPoint(EntityID entity_id, Point* p, Collider* c, PhysicsBody* pb)
{
	auto collisions = m_broad.m_quad_tree.Query(p->GetCenter());

	for (const auto& elt : collisions)
	{
		// no need to check against self

		const auto& obj = m_broad.m_bodies[elt.item];

		if ((c->layer & obj.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && obj.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = obj.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active) // skip trying collision if both are either asleep or disabled
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, p->GetType(), p, c, pb), obj);
		m_broad.m_indices.emplace_back(static_cast<uint32_t>(m_broad.m_pairs.size() - 1));
	}
}

template class BroadShapeHelper<Circle>;
template class BroadShapeHelper<Box>;