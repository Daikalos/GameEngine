#pragma once

#include <SFML/Graphics.hpp>

#include <future>
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
		using ResourcePtr = typename std::unique_ptr<Resource>;

	public:
		ResourceHolder() { }
		~ResourceHolder() { }

		ResourcePtr Load(const std::string& path);

		void Load(const Identifier& id, const std::string& path);
		auto LoadAsync(const Identifier& id, const std::string& path);

		template <class Parameter>
		void Load(const Identifier& id, const std::string& path, const Parameter& second_param);

		Resource& Get(const Identifier& id);
		const Resource& Get(const Identifier& id) const;

	private:
		std::unordered_map<Identifier, ResourcePtr> m_resources;
	};

	template<class Resource, class Identifier>
	ResourceHolder<Resource, Identifier>::ResourcePtr ResourceHolder<Resource, Identifier>::Load(const std::string& path)
	{
		ResourcePtr resource(new Resource());

		if (!resource->loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + path);

		return std::move(resource);
	}

	template<typename Resource, typename Identifier>
	void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string& path)
	{
		auto inserted = m_resources.insert(std::make_pair(id, Load(path));
		assert(inserted.second);
	}

	template<class Resource, class Identifier>
	auto ResourceHolder<Resource, Identifier>::LoadAsync(const Identifier& id, const std::string& path)
	{
		return std::async(std::launch::async, &ResourceHolder<Resource, Identifier>::Load, std::ref(id), std::ref(path));
	}

	template<typename Resource, typename Identifier>
	template<typename Parameter>
	void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string& path, const Parameter& second_param)
	{
		ResourcePtr resource(new Resource());

		if (!resource->loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + path);

		auto inserted = m_resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<typename Resource, typename Identifier>
	Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id)
	{
		return get(id);
	}

	template<typename Resource, typename Identifier>
	const Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id) const
	{
		auto it = m_resources.find(id);
		assert(it != m_resources.end());

		return *it->second.get();
	}

	using TextureHolder = ResourceHolder<sf::Texture, TextureID>;
	using FontHolder = ResourceHolder<sf::Font, FontID>;
}

