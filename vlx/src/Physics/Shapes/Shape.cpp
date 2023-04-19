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

Vector2f Shape::GetPosition() const
{
	return m_aabb.Position();
}

Vector2f Shape::GetCenter() const
{
	return m_aabb.Center();
}

void Shape::UpdateTransform(const GlobalTransform& transform)
{
	sf::Angle new_angle = transform.GetRotation().wrapUnsigned();
	if (m_angle != new_angle)
	{
		m_angle = new_angle;
		m_update = true;
	}
}
