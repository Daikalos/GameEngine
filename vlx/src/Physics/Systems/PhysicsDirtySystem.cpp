#include <Velox/Physics/Systems/PhysicsDirtySystem.h>

using namespace vlx;

PhysicsDirtySystem::PhysicsDirtySystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id, true),

	m_dirty_transform(	entity_admin, LYR_GLOBAL_TRANSFORM),
	m_dirty_physics(	entity_admin, id),

	m_circles(			entity_admin, id),
	m_boxes(			entity_admin, id),
	m_polygons(			entity_admin, id)

{
	m_dirty_transform.Each([](Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_dirty_physics.Each([](Collider& c, Transform& t)
		{
			if (t.m_dirty)
				c.dirty = true;
		});

	m_circles.Each([](Circle& s, Collider& c, ColliderAABB& ab, Transform& t)
		{
			if (c.dirty)
			{
				ab.SetAABB(s.ComputeAABB(t.GetPosition()));
				c.dirty = false;
			}
		});

	m_boxes.Each([](Box& b, Collider& c, ColliderAABB& ab, TransformMatrix& tm)
		{
			if (c.dirty)
			{
				ab.SetAABB(b.ComputeAABB(tm.matrix));
				c.dirty = false;
			}
		});

	m_polygons.Each([](Polygon& p, Collider& c, ColliderAABB& ab, TransformMatrix& tm)
		{
			if (c.dirty)
			{
				ab.SetAABB(p.ComputeAABB(tm.matrix));
				c.dirty = false;
			}
		});

	// TODO: add global transform support for entities that does not hold any physics body

	m_dirty_transform.SetPriority(10000.0f);
}

void PhysicsDirtySystem::FixedUpdate()
{
	Execute(LYR_LOCAL_TRANSFORM); // update local transformation matrices to update AABBs
	Execute();
}
