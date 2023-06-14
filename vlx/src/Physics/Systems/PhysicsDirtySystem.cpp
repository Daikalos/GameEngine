#include <Velox/Physics/Systems/PhysicsDirtySystem.h>

using namespace vlx;

PhysicsDirtySystem::PhysicsDirtySystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id),

	m_dirty_transform(	entity_admin, LYR_TRANSFORM), // hijack the transform layer muhahaha
	m_dirty_physics(	entity_admin, id),

	m_circles(			entity_admin, id),
	m_boxes(			entity_admin, id),
	m_points(			entity_admin, id),
	m_polygons(			entity_admin, id)

{
	m_dirty_transform.Each([this](EntityID entity_id, Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_dirty_physics.Each([this](EntityID entity_id, Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_circles.Each([this](EntityID entity_id, Circle& s, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				s.UpdateAABB(s.ComputeAABB(t));
				// no need to update the orientation matrix
				s.UpdateCenter(t.GetPosition());

				c.dirty = false;
			}
		});

	m_boxes.Each([this](EntityID entity_id, Box& b, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				b.UpdateAABB(b.ComputeAABB(t));
				b.UpdateOrientation(t.GetRotation().wrapUnsigned());
				b.UpdateCenter(t.GetPosition());

				c.dirty = false;
			}
		});

	m_points.Each([this](EntityID entity_id, Point& p, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				p.UpdateAABB(p.ComputeAABB(t));
				// no need to update the orientation matrix
				p.UpdateCenter(t.GetPosition());

				c.dirty = false;
			}
		});

	m_polygons.Each([this](EntityID entity_id, Polygon& p, Collider& c, Transform& t)
		{
			if (c.dirty)
			{
				p.UpdateAABB(p.ComputeAABB(t));
				p.UpdateOrientation(t.GetRotation().wrapUnsigned());
				p.UpdateCenter(t.GetPosition());

				c.dirty = false;
			}
		});

	m_dirty_transform.SetPriority(10000.0f);
}

bool PhysicsDirtySystem::IsRequired() const noexcept
{
	return true;
}

void PhysicsDirtySystem::Start()
{

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
