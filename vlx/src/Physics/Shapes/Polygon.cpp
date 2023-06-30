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

	// gift-wrapping algorithm

	const auto ComparePoints = [](const Vector2f& p1, const Vector2f& p2)
	{
		if (p1.x > p2.x)
			return true;

		if (p1.x == p2.x)
			return p1.y < p2.y;

		return false;
	};

	int hull = 0;
	for (uint32 i = 1; i < points.size(); ++i)
		if (ComparePoints(points[i], points[hull]))
			hull = i;

	std::vector<int> indices(points.size());
	uint32 results = 0;
	uint32 next = 0;

	do
	{
		indices[results++] = hull;

		next = 0;
		for (uint32 i = 1; i < points.size(); ++i)
		{
			if (next == hull)
			{
				next = i;
				continue;
			}

			Vector2f d1 = Vector2f::Direction(points[hull], points[next]);
			Vector2f d2 = Vector2f::Direction(points[hull], points[i]);

			float c = d1.Cross(d2);

			if (c < 0.0f || (c == 0.0f && d2.LengthSq() > d1.LengthSq()))
				next = i;
		}

		hull = next;

	} while (next != indices[0]);

	assert(results != 0);

	m_vertices.resize(results);
	m_normals.resize(results);

	for (uint32 i = 0; i < results; ++i)
		m_vertices[i] = points[indices[i]];

	Vector2f centroid = py::ComputeCentroid(m_vertices);

	for (uint32 i = 0; i < results; ++i)
		m_vertices[i] -= centroid; // shift to centroid

	m_vertices_aabb = py::ComputeAABB(m_vertices);

	for (uint32 i = 0; i < results - 1; ++i) // finally, compute normals
	{
		Vector2f face = Vector2f::Direction(m_vertices[i], m_vertices[i + 1]);

		assert(face.LengthSq() > FLT_EPSILON * FLT_EPSILON);

		m_normals[i] = face.Orthogonal().Normalize();
	}

	m_normals[results - 1] = Vector2f::Direction(m_vertices[results - 1], m_vertices[0]).Orthogonal().Normalize();
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
