#pragma once

#include <span>
#include <optional>
#include <tuple>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Rectangle.hpp>

#include <Velox/VeloxTypes.hpp>

namespace vlx::py
{
	enum WindingOrder : int8
	{
		WO_None = -1,
		WO_Clockwise,
		WO_CounterClockwise
	};

	static RectFloat ComputeAABB(std::span<const Vector2f> vertices)
	{
		float left{FLT_MAX}, top{FLT_MAX}, right{-FLT_MAX}, bot{-FLT_MAX};
		for (uint32 i = 0; i < vertices.size(); ++i) // build aabb from vertices
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

	static Vector2f ComputeCentroid(std::span<const Vector2f> vertices, const Vector2f& fallback = {})
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

	static bool IsSimplePolygon(std::span<const Vector2f> vertices)
	{
		return true;
	}

	static bool ContainsColinearEdges(std::span<const Vector2f> vertices)
	{
		return true;
	}

	static std::tuple<float, WindingOrder> ComputePolygonArea(std::span<const Vector2f> vertices)
	{

	}

	static std::optional<std::vector<Vector2f>> Triangulate(std::span<const Vector2f> vertices)
	{
		if (vertices.size() < 3 || IsSimplePolygon(vertices) || ContainsColinearEdges(vertices))
			return std::nullopt;

		auto [area, order] = ComputePolygonArea(vertices);

		if (order == WO_None)
			return std::nullopt;

		bool reverse = (order == WO_CounterClockwise);

		std::vector<Vector2f> result;



		return std::vector<Vector2f>();
	}
}