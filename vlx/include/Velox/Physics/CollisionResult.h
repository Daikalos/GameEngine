#pragma once

#include <SFML/System/Vector2.hpp>

#include "Bounds.h"

namespace vlx
{
	struct CollisionResult
	{
		bool			collided	{false};
		float			penetration {0.0f};
		sf::Vector2f	normal;
	};
}