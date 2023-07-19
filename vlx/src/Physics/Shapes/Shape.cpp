#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

const RectFloat& Shape::GetAABB() const noexcept	{ return m_aabb; }
const Vector2f& Shape::GetCenter() const noexcept	{ return m_center; }
float Shape::GetRadius() const noexcept				{ return m_radius; }
float Shape::GetRadiusSqr() const noexcept			{ return m_radius_sqr; }

void Shape::UpdateAABB(const RectFloat& aabb)
{
	m_aabb = aabb;
}

void Shape::UpdateCenter(const Vector2f& center)
{
	m_center = center;
}

const Rot2f& ShapeRotatable::GetOrientation() const
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
