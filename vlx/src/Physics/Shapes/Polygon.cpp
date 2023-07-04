#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

Polygon::Polygon() = default;

Polygon::Polygon(std::span<const Vector2f> points)
{
	Set(points);
}

void Polygon::Set(std::span<const Vector2f> points)
{
	assert(points.size() > 2);

	m_vertices = py::GiftWrapVertices(points);
	m_normals.resize(m_vertices.size());

	Vector2f centroid = py::ComputeCentroid(m_vertices);

	for (uint32 i = 0; i < m_vertices.size(); ++i)
		m_vertices[i] -= centroid; // shift to centroid

	m_vertices_aabb = py::ComputeAABB(m_vertices);

	for (uint32 i = 0; i < m_vertices.size() - 1; ++i) // finally, compute normals
	{
		Vector2f face = Vector2f::Direction(m_vertices[i], m_vertices[i + 1]);

		assert(face.LengthSq() > FLT_EPSILON * FLT_EPSILON);

		m_normals[i] = face.Orthogonal().Normalize();
	}

	m_normals[m_vertices.size() - 1] = Vector2f::Direction(m_vertices[m_vertices.size() - 1], m_vertices[0]).Orthogonal().Normalize();
}

auto Polygon::GetVertices() const -> const VectorList&
{
	return m_vertices;
}

auto Polygon::GetNormals() const -> const VectorList&
{
	return m_normals;
}

const RectFloat& Polygon::GetBoundary() const noexcept
{
	return m_vertices_aabb;
}

void Polygon::AdjustBody(PhysicsBody& body) const noexcept
{

}

RectFloat Polygon::ComputeAABB(const Mat4f& matrix) const
{
	return matrix.TransformRect(GetBoundary());
}
