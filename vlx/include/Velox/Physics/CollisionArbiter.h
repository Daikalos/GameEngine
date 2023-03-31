#pragma once

#include <stdint.h>

#include <Velox/System/Vector2.hpp>

#include "CollisionObject.h"
#include "CollisionContact.h"

namespace vlx
{
	struct CollisionArbiter
	{
		static constexpr int MAX_POINTS = 2;

		CollisionObject*	A						{nullptr};
		CollisionObject*	B						{nullptr};

		CollisionContact	contacts[MAX_POINTS];
		std::uint32_t		contacts_count			{0};
			
		float				restitution				{0.0f};	// minimum TODO: allow dev to change mode: min, max, or average
		float				static_friction			{0.0f};	// average
		float				dynamic_friction		{0.0f}; // average
	};
}