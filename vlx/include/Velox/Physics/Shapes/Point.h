#pragma once

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Point final : public Shape
	{
	public:
		static consteval auto GetType() noexcept -> Type;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;
	};

	consteval auto Point::GetType() noexcept -> Type
	{
		return Type::Point;
	}
}