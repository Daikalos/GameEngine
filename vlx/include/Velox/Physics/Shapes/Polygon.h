#pragma once

#include <vector>
#include <span>

#include <Velox/Config.hpp>
#include <Velox/VeloxTypes.hpp>

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
		Vector2f GetLocalCenter() const;

	public:
		constexpr auto GetType() const noexcept -> Type override;

		void AdjustBody(PhysicsBody& body) const noexcept override;
		RectFloat ComputeAABB(const Transform& transform) const override;

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