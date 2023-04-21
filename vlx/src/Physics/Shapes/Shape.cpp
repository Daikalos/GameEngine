#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

const Mat2f& Shape::GetOrientation() const
{
	if (m_update)
	{
		m_orientation.Set(m_angle);
		m_update = false;
	}

	return m_orientation;
}
const RectFloat& Shape::GetAABB() const
{
	return m_aabb;
}
const Vector2f& vlx::Shape::GetCenter() const
{
	return m_center;
}

void Shape::UpdateOrientation(sf::Angle angle)
{
	//sf::Angle new_angle = transform.GetRotation().wrapUnsigned();
	if (m_angle != angle)
	{
		m_angle = angle;
		m_update = true;
	}
}

void Shape::UpdateAABB(const RectFloat& aabb)
{
	m_aabb = aabb;
}

void Shape::UpdateCenter(const Vector2f& center)
{
	m_center = center;
}
