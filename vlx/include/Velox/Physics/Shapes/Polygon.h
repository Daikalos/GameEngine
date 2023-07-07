#pragma once

#include <vector>
#include <span>

#include <Velox/System/Mat4f.hpp>
#include <Velox/Utility/PolygonUtils.h>
#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class VELOX_API Polygon final : public ShapeRotatable
	{
	public:
		using VectorList = std::vector<Vector2f>;

	public:
		Polygon();
		Polygon(std::span<const Vector2f> points);

	public:
		void Set(std::span<const Vector2f> points);

	public:
		auto GetVertices() const -> const VectorList&;
		auto GetNormals() const -> const VectorList&;

		const RectFloat& GetBoundary() const noexcept;

	public:
		constexpr auto GetType() const noexcept -> Type override;
		void AdjustBody(PhysicsBody& body) const noexcept override;

	public:
		RectFloat ComputeAABB(const Mat4f& matrix) const;

	private:
		VectorList	m_vertices;
		VectorList	m_normals;
		RectFloat	m_vertices_aabb;
	};

	constexpr auto Polygon::GetType() const noexcept -> Type
	{
		return Type::Convex;
	}
}