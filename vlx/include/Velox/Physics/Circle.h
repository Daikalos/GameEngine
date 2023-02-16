#pragma once

#include "Shape.h"
#include "CollisionResult.h"

namespace vlx
{
	struct Circle : public Shape
	{
		float radius;

		constexpr Shape::Type GetType() const noexcept override;
	};
}