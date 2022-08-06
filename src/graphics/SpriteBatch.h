#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Batch.h"
#include "Sprite.h"

class SpriteBatch : public sf::Drawable
{
public:
	SpriteBatch();
	~SpriteBatch();

	Batch* find_batch(const Sprite& sprite);

private:
	void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

private:
	std::vector<Batch> _batches;
};

