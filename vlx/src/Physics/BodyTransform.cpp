#include <Velox/Physics/BodyTransform.h>

using namespace vlx;

const Vector2f& BodyTransform::GetPosition() const noexcept
{
	return m_position;
}

sf::Angle BodyTransform::GetRotation() const noexcept
{
	return m_rotation;
}
