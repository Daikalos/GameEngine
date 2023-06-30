#pragma once

#include <Velox/System/Mat4f.hpp>

namespace vlx
{
	struct TransformMatrixInverse
	{
		Mat4f matrix; // inverse transform
	};
}