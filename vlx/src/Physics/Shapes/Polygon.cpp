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

void Polygon::InitializeImpl(PhysicsBody& body) const noexcept
{

}

void Polygon::UpdateAABBImpl(const GlobalTransform& transform)
{

}
