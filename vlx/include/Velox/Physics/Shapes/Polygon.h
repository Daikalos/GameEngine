#pragma once

#include <vector>

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Polygon final : public Shape
	{
	public:
		using VectorList = std::vector<Vector2f>;

	public:
		auto GetVertices() const -> const VectorList&;
		auto GetNormals() const -> const VectorList&;

	public:
		constexpr auto GetType() const noexcept -> Type override;

		VELOX_API void AdjustBody(PhysicsBody& body) const noexcept override;
		VELOX_API Vector2f ComputeCenter(const Vector2f& position) const override;
		VELOX_API RectFloat ComputeAABB(const GlobalTransform& transform) const override;

	private:
		VectorList m_vertices;
		VectorList m_normals;
	};

	constexpr auto Polygon::GetType() const noexcept -> Type
	{
		return Type::Convex;
	}
}