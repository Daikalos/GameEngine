#include <Velox/Physics/BodyLastTransform.h>

using namespace vlx;

const Vector2f& BodyLastTransform::GetPosition() const noexcept
{
	return m_position;
}

sf::Angle BodyLastTransform::GetRotation() const noexcept
{
	return m_rotation;
}