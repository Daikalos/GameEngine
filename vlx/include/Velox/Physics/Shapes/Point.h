#pragma once

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Point final : public Shape
	{
	public:
		constexpr auto GetType() const noexcept -> Type override;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;
	};

	constexpr auto Point::GetType() const noexcept -> Type
	{
		return Type::Point;
	}
}