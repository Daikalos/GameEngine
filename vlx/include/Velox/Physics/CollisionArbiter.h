#pragma once

#include <stdint.h>
#include <array>

#include <Velox/System/Vector2.hpp>

#include "CollisionObject.h"
#include "CollisionContact.h"

namespace vlx
{
	struct CollisionArbiter
	{
		using ContactArray = std::array<CollisionContact, 2>;

		CollisionObject*	A						{nullptr};
		CollisionObject*	B						{nullptr};

		ContactArray		contacts;
		std::uint16_t		contacts_count			{0};
			
		float				restitution				{0.0f};	// minimum TODO: allow user to change mode: min, max, or average
		float				static_friction			{0.0f};	// average
		float				dynamic_friction		{0.0f}; // average
	};
}