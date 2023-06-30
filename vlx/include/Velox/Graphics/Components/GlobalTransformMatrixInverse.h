#pragma once

#include <Velox/System/Mat4f.hpp>

namespace vlx
{
	struct GlobalTransformMatrixInverse
	{
		Mat4f matrix; // inverse transform
	};
}