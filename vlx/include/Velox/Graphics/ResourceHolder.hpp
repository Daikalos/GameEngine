#pragma once

#include <SFML/Graphics.hpp>

#include <future>
#include <memory>
#include <unordered_map>
#include <string_view>
#include <thread>

#include <Velox/Utilities.hpp>

#include "Resources.h"

namespace vlx
{
	template <IsLoadable Resource, Enum Identifier>
	class ResourceHolder : private NonCopyable
	{
	public:
		using ResourcePtr = typename std::unique_ptr<Resource>;

	public:
		void Load(const Identifier& id, const std::string_view path);
		template <class Parameter>
		void Load(const Identifier& id, const std::string_view path, const Parameter& second_param);

		std::future<void> LoadAsync(const Identifier& id, const std::string_view path);

		Resource& Get(const Identifier& id);
		const Resource& Get(const Identifier& id) const;

	private:
		Resource Load(const std::string_view path);
		template<class Parameter>
		Resource Load(const std::string_view path, const Parameter& second_param);

	private:
		std::unordered_map<Identifier, ResourcePtr> m_resources;
		std::mutex m_mutex;
	};

	template<IsLoadable Resource, Enum Identifier>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string_view path)
	{
		Resource resource;

		if (!resource.loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return resource;
	}

	template<IsLoadable Resource, Enum Identifier>
	template<class Parameter>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string_view path, const Parameter& second_param)
	{
		Resource resource;

		if (!resource.loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return resource;
	}

	template<IsLoadable Resource, Enum Identifier>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path)
	{
		ResourcePtr resource = std::make_unique<Resource>(Load(path));
		auto inserted = m_resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<IsLoadable Resource, Enum Identifier>
	template<typename Parameter>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path, const Parameter& second_param)
	{
		ResourcePtr resource = std::make_unique<Resource>(Load(path, second_param));
		auto inserted = m_resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<IsLoadable Resource, Enum Identifier>
	inline std::future<void> ResourceHolder<Resource, Identifier>::LoadAsync(const Identifier& id, const std::string_view path)
	{
		static const auto load = [this](const Identifier& id, const std::string path)
		{
			std::lock_guard lock(m_mutex);
			Load(id, path);
		};

		return std::async(std::launch::async, load, id, std::string(path)); // create local copy of path because it will be later destroyed
	}

	template<IsLoadable Resource, Enum Identifier>
	inline Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id)
	{
		return *m_resources.at(id).get();
	}

	template<IsLoadable Resource, Enum Identifier>
	inline const Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id) const
	{
		return const_cast<ResourceHolder<Resource, Identifier>*>(this)->Get(id);
	}

	using TextureHolder = ResourceHolder<sf::Texture, Texture::ID>;
	using FontHolder = ResourceHolder<sf::Font, Font::ID>;
}

