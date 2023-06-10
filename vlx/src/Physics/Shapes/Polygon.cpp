#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

void Polygon::Set(Vector2f* points, uint32 count)
{
	assert(count > 2);

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
	for (uint32 i = 1; i < count; ++i)
		if (ComparePoints(points[i], points[hull]))
			hull = i;

	std::vector<int> indices(count);
	uint32 results = 0;
	uint32 next = 0;

	do
	{
		indices[results] = hull;

		next = 0;
		for (uint32 i = 1; i < count; ++i)
		{
			if (next == hull)
			{
				next = i;
				continue;
			}

			Vector2f d1 = Vector2f::Direction(points[indices[results]], points[next]);
			Vector2f d2 = Vector2f::Direction(points[indices[results]], points[i]);

			float c = d1.Cross(d2);

			if (c < 0.0f)
				next = i;
			else if (c == 0.0f && d2.LengthSq() > d1.LengthSq())
				next = i;
		}

		++results;
		hull = next;

	} while (next != indices[0]);

	m_vertices.resize(results);
	m_normals.resize(results);

	for (uint32 i = 0; i < results; ++i)
		m_vertices[i] = points[indices[i]];

	for (uint32 i = 0; i < results; ++i)
	{
		uint32 j = (i + 1) < results ? (i + 1) : 0;
		Vector2f face = Vector2f::Direction(m_vertices[i], m_vertices[j]);

		assert(face.LengthSq() > FLT_EPSILON * FLT_EPSILON);

		m_normals[i] = face.Orthogonal().Normalize();
	}

	float left{0}, top{0}, right{0}, bot{0};
	for (uint32 i = 0; i < m_vertices.size(); ++i) // build aabb from vertices
	{
		Vector2f pos = m_vertices[i];

		if (pos.x < left)
			left = pos.x;
		if (pos.y < top)
			top = pos.y;
		if (pos.x > right)
			right = pos.x;
		if (pos.y > bot)
			bot = pos.y;
	}

	m_vertices_aabb = RectFloat(left, top, right - left, bot - top);
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

Vector2f Polygon::GetLocalCenter() const
{
	return m_vertices_aabb.Center();
}

void Polygon::AdjustBody(PhysicsBody& body) const noexcept
{

}

RectFloat Polygon::ComputeAABB(const Transform& transform) const
{
	return transform.GetTransform().TransformRect(GetBoundary());
}
