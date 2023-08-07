#include <Velox/Physics/Collider/ColliderAABB.h>

using namespace vlx;

const RectFloat& ColliderAABB::GetAABB() const noexcept
{
	return m_aabb;
}

void ColliderAABB::SetAABB(const RectFloat& aabb)
{
	m_aabb = aabb;
}
