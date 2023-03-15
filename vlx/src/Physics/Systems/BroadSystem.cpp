#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, const LayerType id, PhysicsSystem& physics_system)
	: SystemAction(entity_admin, id), 

	m_physics_system(		&physics_system),
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

	m_circles_ins.Each([this](const EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &s, &c, nullptr, &lt, &t);
		});

	m_circles_body_ins.Each([this](const EntityID entity_id, Circle& s, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &s, &c, &pb, &lt, &t);
		});

	m_boxes_ins.Each([this](const EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id , &b, &c, nullptr, &lt, &t);
		});

	m_boxes_body_ins.Each([this](const EntityID entity_id, Box& b, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			InsertShape(entity_id, &b, &c, &pb, &lt, &t);
		});

	m_circles_query.Each([this](const EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(&s, &c, nullptr, &lt, &t);
		});

	m_circles_body_query.Each([this](const EntityID entity_id, Circle& s, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			QueryShape(&s, &c, &pb, &lt, &t);
		});

	m_boxes_query.Each([this](const EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(&b, &c, nullptr, &lt, &t);
		});

	m_boxes_body_query.Each([this](const EntityID entity_id, Box& b, Collision& c, PhysicsBody& pb, LocalTransform& lt, Transform& t)
		{
			QueryShape(&b, &c, &pb, &lt, &t);
		});

	m_dirty_transform.SetPriority(90000.0f);

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

void BroadSystem::InsertShape(const EntityID entity_id, Shape* s, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t)
{
	if (c->dirty)
	{
		c->Erase();
		c->Insert(m_quad_tree, s->GetAABB(*t), CollisionObject(s, c, pb, lt, t));

		c->dirty = false;
	}
	else
	{
		c->Update(CollisionObject(s, c, pb, lt, t)); // attempt to update data if already inserted, needed for cases where pointers may be modified
	}
}

void BroadSystem::QueryShape(Shape* s, Collision* c, PhysicsBody* pb, LocalTransform* lt, Transform* t)
{
	auto collisions = m_quad_tree.Query(s->GetAABB(*t));

	for (const auto& collision : collisions)
	{
		if (s == collision.item.shape) // no collision against self
			continue;

		if (!(c->layer & collision.item.collision->layer)) // only matching layer
			continue;

		m_collision_pairs.emplace_back(CollisionObject(s, c, pb, lt, t), collision.item);
		m_collision_indices.emplace_back(static_cast<std::uint32_t>(m_collision_pairs.size() - 1));
	}
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_collision_indices.begin(), m_collision_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_collision_pairs[lhs], &crhs = m_collision_pairs[rhs];

			if (clhs.first.shape < crhs.second.shape)
				return true;

			if (clhs.first.shape == crhs.first.shape)
				return clhs.second.shape < crhs.second.shape;

			return false;
		});

	const auto range = std::ranges::unique(m_collision_indices.begin(), m_collision_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_collision_pairs[lhs], &crhs = m_collision_pairs[rhs];
			return clhs.first.shape == crhs.second.shape || clhs.second.shape == crhs.first.shape;
		});

	m_unique_collisions.assign(range.begin(), range.end());
}
