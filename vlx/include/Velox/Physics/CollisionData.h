#pragma once

#include <stdint.h>

#include <Velox/System/Vector2.hpp>

#include "CollisionObject.h"

namespace vlx
{
	struct CollisionData
	{
		float			penetration			{0.0f};
		Vector2f		normal;

		Vector2f		contacts[2];
		std::uint32_t	contact_count		{0};
			
		float			restitution			{0.0f};	// minimum TODO: allow dev to change mode: min, max, or average
		float			static_friction		{0.0f};	// average
		float			dynamic_friction	{0.0f}; // average
	};
}