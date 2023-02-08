#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Config.hpp>

namespace vlx
{
	struct VELOX_API Velocity : public IComponent
	{
		constexpr Velocity();
		constexpr Velocity(const float x, const float y);
		constexpr Velocity(const sf::Vector2f& vel);

		sf::Vector2f velocity {0, 0};
	};
}