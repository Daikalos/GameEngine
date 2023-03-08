#pragma once

#include <stdint.h>

#include <Velox/System/Vector2.hpp>

#include "CollisionObject.h"

namespace vlx
{
	struct CollisionData
	{
		float			penetration		{0.0f};
		Vector2f		normal;

		Vector2f		contacts[2];
		std::uint32_t	contact_count	{0};

		float			min_restitution	{0.0f};
		float			avg_sfriction	{0.0f};
		float			avg_dfriction	{0.0f};
	};
}