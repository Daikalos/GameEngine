#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

const RectFloat& Shape::GetAABB() const
{
	return m_aabb;
}
const Vector2f& Shape::GetCenter() const
{
	return m_center;
}

void Shape::UpdateAABB(const RectFloat& aabb)
{
	m_aabb = aabb;
}

void Shape::UpdateCenter(const Vector2f& center)
{
	m_center = center;
}

const Mat2f& ShapeRotatable::GetOrientation() const
{
	if (m_update)
	{
		m_orientation.Set(m_angle);
		m_update = false;
	}

	return m_orientation;
}

void ShapeRotatable::UpdateOrientation(sf::Angle angle)
{
	if (m_angle != angle)
	{
		m_angle = angle;
		m_update = true;
	}
}
