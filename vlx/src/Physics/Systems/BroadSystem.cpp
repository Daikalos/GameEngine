#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), 

	m_quad_tree(			{ -4096, -4096, 4096 * 2, 4096 * 2 }), 

	m_dirty_transform(		*m_entity_admin, LYR_TRANSFORM), // intercept the transform layer muhahaha
	m_dirty_physics(		*m_entity_admin, id),

	m_circles_ins(			*m_entity_admin, id),
	m_boxes_ins(			*m_entity_admin, id),
	m_circles_body_ins(		*m_entity_admin, id),
	m_boxes_body_ins(		*m_entity_admin, id),

	m_cleanup(				*m_entity_admin, id),

	m_circles_query(		*m_entity_admin, id),
	m_boxes_query(			*m_entity_admin, id),
	m_circles_body_query(	*m_entity_admin, id),
	m_boxes_body_query(		*m_entity_admin, id)

{
	m_dirty_transform.Each([this](const EntityID entity_id, Collision& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_dirty_physics.Each([this](const EntityID entity_id, Collision& c, LocalTransform& lt)
		{
			if (lt.m_dirty)
				c.dirty = true;
		});

	m_cleanup.OnStart += [this]()
	{
		m_quad_tree.Cleanup();
	};

	m_circles_ins.Each([this](EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &s, &c, nullptr, &lt, &t);
		});

	m_circles_body_ins.Each([this](EntityID entity_id, Circle& s, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &s, &c, &pb, &lt, &t);
		});

	m_boxes_ins.Each([this](EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id , &b, &c, nullptr, &lt, &t);
		});

	m_boxes_body_ins.Each([this](EntityID entity_id, Box& b, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &b, &c, &pb, &lt, &t);
		});

	m_circles_query.Each([this](EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(entity_id, &s, &c, nullptr, &lt, &t);
		});

	m_circles_body_query.Each([this](EntityID entity_id, Circle& s, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			QueryShape(entity_id, &s, &c, &pb, &lt, &t);
		});

	m_boxes_query.Each([this](EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(entity_id, &b, &c, nullptr, &lt, &t);
		});

	m_boxes_body_query.Each([this](EntityID entity_id, Box& b, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			QueryShape(entity_id, &b, &c, &pb, &lt, &t);
		});

	m_dirty_transform.SetPriority(99999.0f);

	m_circles_ins.Exclude<PhysicsBody>();
	m_boxes_ins.Exclude<PhysicsBody>();
	m_circles_query.Exclude<PhysicsBody>();
	m_boxes_query.Exclude<PhysicsBody>();
}

constexpr bool BroadSystem::IsRequired() const noexcept
{
	return true;
}

void BroadSystem::PreUpdate()
{

}

void BroadSystem::Update()
{

}

void BroadSystem::FixedUpdate()
{
	m_collision_pairs.clear();
	m_collision_indices.clear();

	Execute();

	CullDuplicates();
}

void BroadSystem::PostUpdate()
{

}

void BroadSystem::InsertShape(EntityID entity_id, Shape* s, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t)
{
	if (c->dirty)
	{
		c->Erase();
		c->Insert(m_quad_tree, s->GetAABB(*t), entity_id, s, c, pb, lt, t);

		c->dirty = false;
	}
	else
	{
		c->Update(entity_id, s, c, pb, lt, t); // attempt to update data if already inserted, needed for cases where pointers may be modified
	}
}

void BroadSystem::QueryShape(EntityID entity_id, Shape* s, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t)
{
	auto collisions = m_quad_tree.Query(s->GetAABB(*t));

	for (const auto& collision : collisions)
	{
		if (s == collision.item.shape) // no collision against self
			continue;

		if (!(c->layer & collision.item.collision->layer)) // only matching layer
			continue;

		m_collision_pairs.emplace_back(CollisionObject(entity_id, s, c, pb, lt, t), collision.item);
		m_collision_indices.emplace_back(static_cast<std::uint32_t>(m_collision_pairs.size() - 1));
	}
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_collision_indices.begin(), m_collision_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_collision_pairs[lhs], &crhs = m_collision_pairs[rhs];

			if (clhs.first.entity_id < crhs.first.entity_id)
				return true;

			if (clhs.first.entity_id == crhs.first.entity_id)
				return clhs.second.entity_id < crhs.second.entity_id;

			return false;
		});

	const auto [first, last] = std::ranges::unique(m_collision_indices.begin(), m_collision_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_collision_pairs[lhs], & crhs = m_collision_pairs[rhs];

			return	clhs.first.entity_id == crhs.second.entity_id && 
					clhs.second.entity_id == crhs.first.entity_id;
		});

	m_collision_indices.erase(first, last);

	//std::uint32_t i = 0;
	//while (i < m_collision_indices.size())
	//{
	//	std::uint32_t i0 = m_collision_indices[i++];
	//	m_unique_collisions.emplace_back(i0);

	//	while (i < m_collision_indices.size())
	//	{
	//		std::uint32_t i1 = m_collision_indices[i];

	//		if (m_collision_pairs[i0].first.entity_id != m_collision_pairs[i1].second.entity_id || 
	//			m_collision_pairs[i0].second.entity_id != m_collision_pairs[i1].first.entity_id)
	//			break;

	//		++i;
	//	}
	//}
}
