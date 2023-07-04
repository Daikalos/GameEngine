#pragma once

#include <span>
#include <optional>

#include <SFML/Graphics/Vertex.hpp>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Rectangle.hpp>

#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

namespace vlx::py
{
	enum WindingOrder : int8
	{
		WO_None = -1,
		WO_Clockwise,
		WO_CounterClockwise
	};

	NODISC VELOX_API RectFloat ComputeAABB(std::span<const sf::Vertex> vertices);
	NODISC VELOX_API RectFloat ComputeAABB(std::span<const Vector2f> vertices);

	NODISC VELOX_API Vector2f ComputeCentroid(std::span<const Vector2f> vertices, const Vector2f& fallback = {});

	NODISC VELOX_API std::vector<Vector2f> GiftWrapVertices(std::span<const Vector2f> vertices);

	NODISC VELOX_API bool IsSimplePolygon(std::span<const Vector2f> vertices);

	NODISC VELOX_API bool ContainsColinearEdges(std::span<const Vector2f> vertices);

	NODISC VELOX_API WindingOrder ComputePolygonWindingOrder(std::span<const Vector2f> vertices);

	NODISC VELOX_API float ComputePolygonArea(std::span<const Vector2f> vertices);

	NODISC VELOX_API bool IsPointInTriangle(const Vector2f& p, const Vector2f& a, const Vector2f& b, const Vector2f& c);

	NODISC VELOX_API std::optional<std::vector<uint32>> Triangulate(std::span<const Vector2f> vertices);
}