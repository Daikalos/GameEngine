#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>

namespace vlx::gui
{
	class Text : public IComponent
	{
		sf::Text text;
	};
}

