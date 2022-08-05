#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>

namespace sfpl
{
	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		sf::Texture* request_texture(std::string name, sf::Texture* fallback = nullptr) const;
		sf::Font* request_font(std::string name, sf::Font* fallback = nullptr) const;

		void load_texture(std::string name, std::string path);
		void load_font(std::string name, std::string path);

		void load_textures();
		void load_fonts();

		void clean_up();

	private:
		std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
		std::unordered_map<std::string, std::unique_ptr<sf::Font>> _fonts;
	};
}

