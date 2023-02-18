#pragma once

#include "Shape.h"
#include "CollisionResult.h"

namespace vlx
{
	struct Circle : public Shape
	{
		float radius{0.0f};

		constexpr Shape::Type GetType() const noexcept override;
	};
}