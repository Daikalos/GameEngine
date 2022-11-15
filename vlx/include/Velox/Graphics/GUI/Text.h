#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>

namespace vlx::gui
{
	struct Text : public IComponent
	{
		sf::Text Text;
	};
}

