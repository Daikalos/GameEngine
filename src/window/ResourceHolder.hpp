#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>

#include <iostream>
#include <filesystem>

#include "../utilities/NonCopyable.h"

namespace fge
{
	enum class TextureID
	{

	};

	enum class FontID
	{

	};

	template <class Resource, class Identifier>
	class ResourceHolder : private NonCopyable
	{
	public:
		using ptr = std::unique_ptr<Resource>;

	public:
		ResourceHolder() { }
		~ResourceHolder() { }

		void load(const Identifier& id, const std::string& path);

		template <class Parameter>
		void load(const Identifier& id, const std::string& path, const Parameter& second_param);

		sf::Texture& get(const Identifier& id);
		const sf::Texture& get(const Identifier& id) const;

	private:
		std::unordered_map<Identifier, ptr> _resources;
	};

	template<typename Resource, typename Identifier>
	void ResourceHolder<Resource, Identifier>::load(const Identifier& id, const std::string& path)
	{
		std::unique_ptr<Resource> resource(new Resource());

		if (!resource->loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + path);

		auto inserted = _resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<typename Resource, typename Identifier>
	template<typename Parameter>
	void ResourceHolder<Resource, Identifier>::load(const Identifier& id, const std::string& path, const Parameter& second_param)
	{
		std::unique_ptr<Resource> resource(new Resource());

		if (!resource->loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + path);

		auto inserted = _resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<typename Resource, typename Identifier>
	sf::Texture& ResourceHolder<Resource, Identifier>::get(const Identifier& id)
	{
		return get(id);
	}

	template<typename Resource, typename Identifier>
	const sf::Texture& ResourceHolder<Resource, Identifier>::get(const Identifier& name) const
	{
		auto it = _resources.find(name);
		assert(it != _resources.end());

		return *it->second.get();
	}

	using TextureHolder = ResourceHolder<sf::Texture, TextureID>;
	using FontHolder = ResourceHolder<sf::Font, FontID>;
}

