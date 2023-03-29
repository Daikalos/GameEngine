#pragma once

#include <vector>

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class VELOX_API Polygon final : public ShapeCRTP<Polygon>
	{
	public:

	public:
		constexpr auto GetType() const noexcept -> Type;

	public:
		void InitializeImpl(PhysicsBody& body) const noexcept;
		void UpdateAABBImpl(const Transform& transform);
		void UpdateTransformImpl(const Transform& transform);

	private:
		std::vector<Vector2f> m_vertices;
		std::vector<Vector2f> m_normals;
	};
}