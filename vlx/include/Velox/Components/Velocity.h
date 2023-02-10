#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Config.hpp>

namespace vlx
{
	struct Velocity : public IComponent, public sf::Vector2f
	{
		using sf::Vector2f::Vector2;
	};
}