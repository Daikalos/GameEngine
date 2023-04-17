#pragma once

#include <stdint.h>
#include <bitset>

namespace vlx
{
	struct CollisionLayer
	{
		operator size_t () const { return layer; }

		size_t layer {1};
	};
}