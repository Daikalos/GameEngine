#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

auto Polygon::GetVertices() const -> const VectorList&
{
	return m_vertices;
}

auto Polygon::GetNormals() const -> const VectorList&
{
	return m_normals;
}

void Polygon::AdjustBody(PhysicsBody& body) const noexcept
{

}

Vector2f Polygon::ComputeCenter(const Vector2f& position) const
{
	return Vector2f();
}

RectFloat Polygon::ComputeAABB(const GlobalTransform& transform) const
{
	return RectFloat();
}
