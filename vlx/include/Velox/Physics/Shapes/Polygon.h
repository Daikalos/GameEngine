#pragma once

#include "Shape.h"

#include <Velox/Algorithms/SmallVector.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API Polygon final : public Shape
	{
	public:

	public:
		constexpr Type GetType() const noexcept override;

	public:
		void Initialize(PhysicsBody& body) const noexcept override;
		RectFloat GetAABB() const override;

	private:
		SmallVector<Vector2f> m_vertices;
		SmallVector<Vector2f> m_normals;
	};
}