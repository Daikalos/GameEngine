#include <Velox/Graphics/Components/GlobalTransformRotation.h>

using namespace vlx;

sf::Angle GlobalTransformRotation::GetRotation() const
{
	return m_rotation;
}
