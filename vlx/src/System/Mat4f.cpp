#include <Velox/System/Mat4f.hpp>

using namespace vlx;

Mat4f& Mat4f::Rotate(sf::Angle angle)
{
	m_transform.rotate(angle);
	return *this;
}

Mat4f& Mat4f::Rotate(sf::Angle angle, const Vector2f& center)
{
	m_transform.rotate(angle, center);
	return *this;
}