#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>

#include <iostream>
#include <filesystem>

namespace fge
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

		// loads all valid textures at given path, the name that is assigned will be based on the file name
		//
		void load_textures(std::string path, std::string prefix = std::string());

		// loads all valid fonts at given path, the name that is assigned will be based on the file name
		//
		void load_fonts(std::string path, std::string prefix = std::string());

		void clean_up();

	private:
		std::unordered_map<std::string, std::unique_ptr<sf::Texture>> _textures;
		std::unordered_map<std::string, std::unique_ptr<sf::Font>> _fonts;
	};
}

