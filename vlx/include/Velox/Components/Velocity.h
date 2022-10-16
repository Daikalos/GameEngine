#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Config.hpp>

namespace vlx
{
	struct VELOX_API Velocity : IComponent
	{
		Velocity();
		Velocity(const sf::Vector2f& vel);

		sf::Vector2f velocity;
	};
}