#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), 
	m_quad_tree({ -4096, -4096, 4096, 4096 }), 
	m_dirty_transform(*m_entity_admin, LYR_TRANSFORM), // intercept the transform layer muhahaha
	m_dirty_physics(*m_entity_admin, LYR_BROAD_PHASE),
	m_circles(*m_entity_admin, LYR_BROAD_PHASE),
	m_boxes(*m_entity_admin, LYR_BROAD_PHASE),
	m_broad_collisions(*m_entity_admin, LYR_BROAD_PHASE)
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

	m_broad_collisions.Each([this](const EntityID entity_id, Collision& c, LocalTransform& lt, Transform& t)
		{

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
	Execute();
}

void BroadSystem::PostUpdate()
{

}

void BroadSystem::InsertShape(Shape& s, Collision& c, LocalTransform& lt, Transform& t)
{
	if (c.m_dirty)
	{
		c.Erase();
		c.Insert(m_quad_tree, { &s, &c, &lt, &t }, s.GetAABB());

		c.m_dirty = false;
	}
}
