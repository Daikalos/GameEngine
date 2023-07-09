#include <Velox/Utility/PolygonUtils.h>

#include <numeric>

#include <Velox/Utility/ArithmeticUtils.h>
#include <Velox/Structures/PriorityQueue.hpp>
#include <Velox/Structures/RBTree.hpp>

#include <Velox/Utility/Random.h>

namespace vlx::py
{
	RectFloat ComputeAABB(std::span<const sf::Vertex> vertices)
	{
		float left{ FLT_MAX }, top{ FLT_MAX }, right{ -FLT_MAX }, bot{ -FLT_MAX };
		for (uint64 i = 0; i < vertices.size(); ++i) // build aabb from vertices
		{
			Vector2f pos = vertices[i].position;

			if (pos.x < left)
				left = pos.x;
			if (pos.x > right)
				right = pos.x;

			if (pos.y < top)
				top = pos.y;
			if (pos.y > bot)
				bot = pos.y;
		}

		assert(left != FLT_MAX && top != FLT_MAX && right != -FLT_MAX && bot != -FLT_MAX);

		return RectFloat(left, top, right - left, bot - top);
	}

	RectFloat ComputeAABB(std::span<const Vector2f> vertices)
	{
		float left{ FLT_MAX }, top{ FLT_MAX }, right{ -FLT_MAX }, bot{ -FLT_MAX };
		for (uint64 i = 0; i < vertices.size(); ++i) // build aabb from vertices
		{
			Vector2f pos = vertices[i];

			if (pos.x < left)
				left = pos.x;
			if (pos.x > right)
				right = pos.x;

			if (pos.y < top)
				top = pos.y;
			if (pos.y > bot)
				bot = pos.y;
		}

		assert(left != FLT_MAX && top != FLT_MAX && right != -FLT_MAX && bot != -FLT_MAX);

		return RectFloat(left, top, right - left, bot - top);
	}

	Vector2f ComputeCentroid(std::span<const Vector2f> vertices, const Vector2f& fallback)
	{
		switch (vertices.size())
		{
		case 0:
			assert(false);
			return Vector2f();
		case 1:
			return vertices[0];
		case 2:
			return (vertices[0] + vertices[1]) / 2.0f;
		default:
			Vector2f centroid;
			float total_area = 0.0f;

			Vector2f prev = vertices.back();
			for (std::size_t i = 0; i < vertices.size(); ++i)
			{
				const Vector2f curr = vertices[i];
				const float area = prev.Cross(curr);

				total_area += area;
				centroid += (curr + prev) * area;

				prev = curr;
			}

			if (total_area != 0.0f)
				return centroid / (3.0f * total_area);

			return fallback; // otherwise, return fallback
		}
	}

	std::vector<Vector2f> GiftWrapVertices(std::span<const Vector2f> vertices)
	{
		std::vector<Vector2f> result;

		if (vertices.size() < 2)
			return result;

		const auto ComparePoints = [](const Vector2f& p1, const Vector2f& p2)
		{
			if (p1.x > p2.x)
				return true;

			if (p1.x == p2.x)
				return p1.y < p2.y;

			return false;
		};

		std::vector<uint64> indices(vertices.size());

		uint64 hull = 0;
		for (uint64 i = 1; i < vertices.size(); ++i)
			if (ComparePoints(vertices[i], vertices[hull]))
				hull = i;

		uint64 count = 0;
		uint64 next = 0;

		do
		{
			indices[count++] = hull;

			next = 0;
			for (uint64 i = 1; i < vertices.size(); ++i)
			{
				if (next == hull)
				{
					next = i;
					continue;
				}

				Vector2f d1 = Vector2f::Direction(vertices[hull], vertices[next]);
				Vector2f d2 = Vector2f::Direction(vertices[hull], vertices[i]);

				float c = d1.Cross(d2);

				if (c < 0.0f || (c == 0.0f && d2.LengthSq() > d1.LengthSq()))
					next = i;
			}

			hull = next;

		} while (next != indices[0]);

		if (count < 2)
			return result;

		result.resize(count);
		for (uint64 i = 0; i < count; ++i)
			result[i] = vertices[indices[i]];

		return result;
	}

	bool IsSimplePolygon(std::span<const Vector2f> vertices)
	{
		PriorityQueue<Vector2f> queue;
		RBTree<float> test;
		
		for (int i = 0; i < 1000; ++i)
			test.Insert(rnd::random(0.0f, 1000.0f));

		return false;
	}

	bool ContainsColinearEdges(std::span<const Vector2f> vertices)
	{
		return false;
	}

	WindingOrder ComputePolygonWindingOrder(std::span<const Vector2f> vertices)
	{
		uint64 count = vertices.size();

		if (count < 3)
			return WO_None;

		uint64 min_vertex = 0;

		float min_y = FLT_MAX;
		float min_x = FLT_MAX;

		for (uint64 i = 0; i < vertices.size(); ++i)
		{
			const Vector2f& v = vertices[i];
			if (v.y < min_y || (v.y == min_y && v.x < min_x))
			{
				min_vertex = i;
				min_y = v.y;
				min_x = v.x;
			}
		}

		assert((min_y != FLT_MAX && min_x != FLT_MAX) && "This should not happen");

		const Vector2f& a = vertices[min_vertex];
		const Vector2f& b = vertices[au::WrapLower<int64>(min_vertex - 1, count)];
		const Vector2f& c = vertices[au::WrapUpper<int64>(min_vertex + 1, count)];

		Vector2f ab = Vector2f::Direction(a, b);
		Vector2f ac = Vector2f::Direction(a, c);

		float cross = ab.Cross(ac);

		if (cross == 0.0f)
			return WO_None;

		return (cross > 0.0f) ? WO_Clockwise : WO_CounterClockwise;
	}

	float ComputePolygonArea(std::span<const Vector2f> vertices)
	{
		float total_area = 0.0f;

		Vector2f prev = vertices.back();
		for (std::size_t i = 0; i < vertices.size(); ++i)
		{
			const Vector2f curr = vertices[i];
			const float area = prev.Cross(curr);

			total_area += area;

			prev = curr;
		}

		return std::copysignf(total_area, 1.0f) / 2.0f;
	}

	bool IsPointInTriangle(const Vector2f& p, const Vector2f& a, const Vector2f& b, const Vector2f& c)
	{
		Vector2f ab = Vector2f::Direction(a, b);
		Vector2f bc = Vector2f::Direction(b, c);
		Vector2f ca = Vector2f::Direction(c, a);

		Vector2f ap = Vector2f::Direction(a, p);
		Vector2f bp = Vector2f::Direction(b, p);
		Vector2f cp = Vector2f::Direction(c, p);

		float c1 = ab.Cross(ap);
		float c2 = bc.Cross(bp);
		float c3 = ca.Cross(cp);

		return !(c1 > 0.0f || c2 > 0.0f || c3 > 0.0f);
	}

	std::optional<std::vector<uint64>> Triangulate(std::span<const Vector2f> vertices)
	{
		if (vertices.size() < 3 || IsSimplePolygon(vertices) || ContainsColinearEdges(vertices))
			return std::nullopt;

		WindingOrder order = ComputePolygonWindingOrder(vertices);

		if (order == WO_None)
			return std::nullopt;

		//bool reverse = (order == WO_CounterClockwise);

		std::vector<uint64> open(vertices.size());
		std::iota(open.begin(), open.end(), 0);

		uint64 total_triangle_count = vertices.size() - 2;
		uint64 total_triangle_index_count = total_triangle_count * 3;

		std::vector<uint64> indices;
		indices.reserve(total_triangle_index_count);

		while (open.size() > 3)
		{
			for (uint64 i = 0; i < open.size(); ++i)
			{
				uint64 a = open[i];
				uint64 b = open[au::WrapLower<int64>(i - 1, open.size())];
				uint64 c = open[au::WrapUpper<int64>(i + 1, open.size())];

				const Vector2f& va = vertices[a];
				const Vector2f& vb = vertices[b];
				const Vector2f& vc = vertices[c];

				Vector2f ab = Vector2f::Direction(va, vb);
				Vector2f ac = Vector2f::Direction(vc, va);

				if (ab.Cross(ac) < 0.0f) // is ear test vertex convex
					continue;

				bool is_ear = true;
				for (uint64 j = 0; j < open.size(); ++j) // does ear contain any vertex, TODO: Optimize
				{
					if (j == a || j == b || j == c)
						continue;

					Vector2f p = vertices[j];

					if (IsPointInTriangle(p, vb, va, vc))
					{
						is_ear = false;
						break;
					}
				}

				if (is_ear)
				{
					indices.emplace_back(b);
					indices.emplace_back(a);
					indices.emplace_back(c);

					open.erase(open.begin() + i);

					break;
				}
			}
		}

		indices.emplace_back(open[0]);
		indices.emplace_back(open[1]);
		indices.emplace_back(open[2]);

		return indices;
	}
}