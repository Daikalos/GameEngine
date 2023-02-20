#pragma once

#include "Shape.h"
#include <Velox/Utilities/Rectangle.hpp>

namespace vlx
{
	struct Box : public Shape
	{
		RectFloat rectangle;

		constexpr Type GetType() const noexcept override
		{
			return Shape::Box;
		}
	};
}