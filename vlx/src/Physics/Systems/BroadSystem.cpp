#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), 
	m_quad_tree({ -4096, -4096, 4096, 4096 }), 
	m_dirty_transform(*m_entity_admin, LYR_TRANSFORM), // intercept the transform layer muhahaha
	m_dirty_physics(*m_entity_admin, LYR_BROAD_PHASE),
	m_circles(*m_entity_admin, LYR_BROAD_PHASE),
	m_boxes(*m_entity_admin, LYR_BROAD_PHASE),
	m_cleanup(*m_entity_admin, LYR_BROAD_PHASE),
	m_circles_broad(*m_entity_admin, LYR_BROAD_PHASE),
	m_boxes_broad(*m_entity_admin, LYR_BROAD_PHASE)
{
	m_dirty_transform.Each([this](const EntityID entity_id, Collision& c, Transform& t)
		{
			if (t.m_dirty)
				c.m_dirty = true;
		});

	m_dirty_physics.Each([this](const EntityID entity_id, Collision& c, LocalTransform& lt)
		{
			if (lt.m_dirty)
				c.m_dirty = true;
		});

	m_circles.Each([this](const EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(s, c, lt, t);
		});

	m_boxes.Each([this](const EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			InsertShape(b, c, lt, t);
		});

	m_cleanup.All([this](std::span<const EntityID> entity_id, Collision* c, LocalTransform* lt, Transform* t)
		{
			m_quad_tree.Cleanup();
		});

	m_circles_broad.Each([this](const EntityID entity_id, Circle& s, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(s, c, lt, t);
		});

	m_boxes_broad.Each([this](const EntityID entity_id, Box& b, Collision& c, LocalTransform& lt, Transform& t)
		{
			QueryShape(b, c, lt, t);
		});

	m_dirty_transform.SetPriority(90000.0f);
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
	Execute();
	CullDuplicates();
}

void BroadSystem::PostUpdate()
{

}

void BroadSystem::InsertShape(Shape& s, Collision& c, LocalTransform& lt, Transform& t)
{
	if (c.m_dirty)
	{
		c.Erase();
		c.Insert(m_quad_tree, { &s, &c, &lt, &t }, s.GetAABB() + t.GetPosition());

		c.m_dirty = false;
	}
}

void BroadSystem::QueryShape(Shape& shape, Collision& c, LocalTransform& lt, Transform& t)
{
	auto collisions = m_quad_tree.Query(shape.GetAABB() + t.GetPosition());

	for (const auto& collision : collisions)
	{
		if (&shape == std::get<0>(collision.item)) // no collision against self
			continue;

		if (!(c.Layer & std::get<1>(collision.item)->Layer))
			continue;

		m_collision_pairs.emplace_back(CollisionObject(&shape, &c, &lt, &t), CollisionObject(
			std::get<0>(collision.item), 
			std::get<1>(collision.item), 
			std::get<2>(collision.item), 
			std::get<3>(collision.item)));
	}
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_collision_pairs.begin(), m_collision_pairs.end(), [](auto& lhs, auto& rhs)
		{
			if (lhs.A.Shape < rhs.A.Shape)
				return true;

			if (lhs.A.Shape == rhs.A.Shape)
				return lhs.B.Shape == rhs.B.Shape;

			return false;
		});

	std::puts(std::to_string(m_collision_pairs.size()).c_str());
}
