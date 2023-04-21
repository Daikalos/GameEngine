#include <Velox/Physics/Systems/PhysicsDirtySystem.h>

using namespace vlx;

PhysicsDirtySystem::PhysicsDirtySystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id),

	m_dirty_transform(	entity_admin, LYR_TRANSFORM), // hijack the transform layer muhahaha
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
			if (t.m_dirty) // TODO: fatten aabbs instead to reduce number of insert/erase
				c.dirty = true;
		});

	m_circles_aabb.Each([this](EntityID entity_id, Circle& s, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				s.UpdateAABB(s.ComputeAABB(gt));
				// no need to update the orientation matrix
				s.UpdateCenter(s.ComputeCenter(gt.GetPosition()));
			}
		});

	m_boxes_aabb.Each([this](EntityID entity_id, Box& b, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				b.UpdateAABB(b.ComputeAABB(gt));
				b.UpdateOrientation(gt.GetRotation().wrapUnsigned());
				b.UpdateCenter(b.ComputeCenter(gt.GetPosition()));
			}
		});

	m_points_aabb.Each([this](EntityID entity_id, Point& p, Collider& c, GlobalTransform& gt)
		{
			if (c.dirty)
			{
				p.UpdateAABB(p.ComputeAABB(gt));
				// no need to update the orientation matrix
				p.UpdateCenter(p.ComputeCenter(gt.GetPosition()));
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
