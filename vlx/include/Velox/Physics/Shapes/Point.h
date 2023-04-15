#pragma once

#include "Shape.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class Point final : public ShapeCRTP<Point>
	{
	public:
		constexpr auto GetType() const noexcept -> Type;
		VELOX_API void InitializeImpl(PhysicsBody& body) const;
		VELOX_API void UpdateAABBImpl(const Transform& transform);
	};

	constexpr auto Point::GetType() const noexcept -> Type
	{
		return Type::Point;
	}
}