#pragma once

#include <vector>

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class VELOX_API Polygon final : public Shape
	{
	public:

	public:
		constexpr Type GetType() const noexcept override;
		void SetAABB(const Transform& transform) override;
		void Initialize(PhysicsBody& body) const noexcept override;

	private:
		std::vector<Vector2f> m_vertices;
		std::vector<Vector2f> m_normals;
	};
}