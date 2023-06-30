#include <Velox/Physics/PhysicsBodyTransform.h>

using namespace vlx;

const Vector2f& PhysicsBodyTransform::GetLastPosition() const noexcept
{
	return m_last_pos;
}

sf::Angle PhysicsBodyTransform::GetLastRotation() const noexcept
{
	return m_last_rot;
}
