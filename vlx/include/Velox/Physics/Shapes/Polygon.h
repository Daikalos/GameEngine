#pragma once

#include <vector>

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Polygon final : public ShapeCRTP<Polygon>
	{
	public:
		using VectorList = std::vector<Vector2f>;

	public:
		auto GetVertices() const -> const VectorList&;
		auto GetNormals() const -> const VectorList&;

	public:
		constexpr auto GetType() const noexcept -> Type;
		VELOX_API void InitializeImpl(PhysicsBody& body) const noexcept;
		VELOX_API void UpdateAABBImpl(const GlobalTransform& transform);

	private:
		VectorList m_vertices;
		VectorList m_normals;
	};

	constexpr auto Polygon::GetType() const noexcept -> Type
	{
		return Type::Convex;
	}
}