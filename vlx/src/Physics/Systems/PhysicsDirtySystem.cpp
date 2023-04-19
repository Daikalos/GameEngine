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
	m_dirty_transform.Each([this](EntityID entity_id, Collider& c, GlobalTransform& gt)
		{
			if (gt.m_dirty)
				c.dirty = true;
		});

	m_dirty_physics.Each([this](EntityID entity_id, Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_circles_aabb.Each([this](EntityID entity_id, Circle& s, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				s.UpdateAABB(gt);
				s.UpdateTransform(gt);
			}
		});

	m_boxes_aabb.Each([this](EntityID entity_id, Box& b, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				b.UpdateAABB(gt);
				b.UpdateTransform(gt);
			}
		});

	m_points_aabb.Each([this](EntityID entity_id, Point& p, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				p.UpdateAABB(gt);
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
