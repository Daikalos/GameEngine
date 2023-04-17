#include <Velox/Physics/Systems/PhysicsDirtySystem.h>

using namespace vlx;

PhysicsDirtySystem::PhysicsDirtySystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id),

	m_dirty_transform(	entity_admin, LYR_TRANSFORM), // intercept the transform layer muhahaha
	m_dirty_physics(	entity_admin, id),

	m_circles_aabb(		entity_admin, id),
	m_boxes_aabb(		entity_admin, id),
	m_points_aabb(		entity_admin, id)

{
	m_dirty_transform.Each([this](EntityID entity_id, Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_dirty_physics.Each([this](EntityID entity_id, Collider& c, LocalTransform& lt)
		{
			if (lt.m_dirty)
				c.dirty = true;
		});

	m_circles_aabb.Each([this](EntityID entity_id, Circle& s, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				s.UpdateAABB(t);
				s.UpdateTransform(t);
			}
		});

	m_boxes_aabb.Each([this](EntityID entity_id, Box& b, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				b.UpdateAABB(t);
				b.UpdateTransform(t);
			}
		});

	m_points_aabb.Each([this](EntityID entity_id, Point& p, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				p.UpdateAABB(t);
				// no need to update the orientation matrix
			}
		});

	m_dirty_transform.SetPriority(99999.0f);
}

bool PhysicsDirtySystem::IsRequired() const noexcept
{
	return true;
}

void PhysicsDirtySystem::PreUpdate()
{

}

void PhysicsDirtySystem::Update()
{
	
}

void PhysicsDirtySystem::FixedUpdate()
{
	Execute();
}

void PhysicsDirtySystem::PostUpdate()
{

}
