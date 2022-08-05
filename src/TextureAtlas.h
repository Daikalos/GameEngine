#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

namespace fge
{
	class TextureAtlas
	{
	public:

	private:
		struct Atlas
		{
			Atlas() : _texture(new sf::Texture()) {}

			std::unique_ptr<sf::Texture> _texture;
			std::unordered_map<std::string, sf::IntRect> _texture_table;
		};

		std::vector<Atlas> _atlas;
	};
}

