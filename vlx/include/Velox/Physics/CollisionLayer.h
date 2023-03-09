#pragma once

#include <stdint.h>

namespace vlx
{
	struct CollisionLayer
	{
		operator std::uint32_t& () { return layer; }
		operator const std::uint32_t& () const { return layer; }

		std::uint32_t layer {1};
	};
}