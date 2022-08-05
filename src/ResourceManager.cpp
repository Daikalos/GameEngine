#include "ResourceManager.h"

using namespace fge;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	clean_up();
}

sf::Texture* ResourceManager::request_texture(std::string name, sf::Texture* fallback) const
{
	auto it = _textures.find(name);

	if (it != _textures.end())
		return it->second.get();
	
	return fallback;
}

sf::Font* ResourceManager::request_font(std::string name, sf::Font* fallback) const
{
	auto it = _fonts.find(name);

	if (it != _fonts.end())
		return it->second.get();

	return fallback;
}
void ResourceManager::load_textures()
{

}
void ResourceManager::load_fonts()
{

}

void ResourceManager::clean_up()
{
	_textures.clear();
	_fonts.clear();
}

void ResourceManager::load_texture(std::string name, std::string path)
{
	_textures[name] = std::make_unique<sf::Texture>();

	if (!_textures[name]->loadFromFile(path))
	{
		_textures.erase(name);
		return;
	}
}
void ResourceManager::load_font(std::string name, std::string path)
{
	_fonts[name] = std::make_unique<sf::Font>();

	if (!_fonts[name]->loadFromFile(path))
	{
		_fonts.erase(name);
		return;
	}
}
