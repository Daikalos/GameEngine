#pragma once

#include <SFML/Graphics.hpp>

namespace fge
{
	struct Component // most basic component
	{
		sf::Vector2f _position;
		sf::Vector2f _velocity;
	};
}

