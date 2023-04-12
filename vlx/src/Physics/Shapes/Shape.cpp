#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

const Mat2f& Shape::GetTransform() const
{
	return m_transform;
}

const Mat2f& Shape::GetInverseTransform() const
{
	if (m_update_inverse)
	{
		m_inverse_transform = m_transform.GetInverse();
		m_update_inverse = false;
	}

	return m_inverse_transform;
}

const RectFloat& Shape::GetAABB() const
{
	return m_aabb;
}

Vector2f Shape::GetCenter() const
{
	return m_aabb.Center();
}

void Shape::UpdateTransform(const Transform& transform)
{
	m_transform.Set(transform.GetRotation());
	m_update_inverse = true;
}
