#include <Velox/Graphics/Systems/LocalTransformSystem.h>

using namespace vlx;

LocalTransformSystem::LocalTransformSystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id, true), m_sync(entity_admin, id)
{
	m_sync.Each(
		[](Transform& t, TransformMatrix& tm)
		{
			// TODO: consider implementing TransformChanging component that tracks changes instead of doing it brute-force as it is now

			if (t.m_update_rot)
			{
				tm.matrix.Rebuild(t.GetScale(), t.GetRotation());
				t.m_update_rot = false;
			}

			if (t.m_update_pos)
			{
				tm.matrix.Rebuild(t.GetPosition(), t.GetOrigin());
				t.m_update_pos = false;
			}
		});
}

void LocalTransformSystem::Update()
{
	Execute();
}
