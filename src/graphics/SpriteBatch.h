#pragma once

#include <SFML/Graphics.hpp>

class SpriteBatch : public sf::Drawable
{
public:
	SpriteBatch()
	{

	}

private:
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
};

