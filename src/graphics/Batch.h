#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Batch
{
public:

private:
	const sf::Texture* _texture;
	int _layer;

	std::vector<sf::Vertex> _vertices;
};

