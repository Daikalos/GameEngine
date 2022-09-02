#pragma once

#include <SFML/Graphics.hpp>

namespace vlx
{
	struct Transform
	{
		sf::Vector2f position;
		sf::Vector2f scale;
		float rotation{ 0.0f };
	};
}