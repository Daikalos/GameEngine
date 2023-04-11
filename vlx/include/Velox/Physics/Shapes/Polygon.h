#pragma once

#include <vector>

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Polygon final : public ShapeCRTP<Polygon>
	{
	public:

	public:
		constexpr auto GetType() const noexcept -> Type;
		VELOX_API void InitializeImpl(PhysicsBody& body) const noexcept;
		VELOX_API void UpdateAABBImpl(const Transform& transform);
		VELOX_API void UpdateTransformImpl(const Transform& transform);

	private:
		std::vector<Vector2f> m_vertices;
		std::vector<Vector2f> m_normals;
	};

	constexpr auto Polygon::GetType() const noexcept -> Type
	{
		return Type::Convex;
	}
}