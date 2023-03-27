#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

const RectFloat& Shape::GetAABB() const
{
	return m_aabb;
}

Vector2f Shape::GetCenter() const
{
	return m_aabb.Center();
}
