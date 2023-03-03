#pragma once

#include <future>
#include <memory>
#include <unordered_map>
#include <string_view>
#include <thread>

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>

#include "Resources.h"

namespace vlx
{
	/// Holds resources, for example, fonts, textures, sounds.
	///
	template <IsLoadable Resource, Enum Identifier>
	class ResourceHolder : private NonCopyable
	{
	public:
		using ResourcePtr = typename std::unique_ptr<Resource>;

	public:
		void Load(const Identifier& id, const std::string_view path);
		template <class Parameter>
		void Load(const Identifier& id, const std::string_view path, const Parameter& second_param);

		std::future<void> LoadAsync(const Identifier& id, const std::string& path);

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
		Resource resource{};

		if (!resource.loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return resource;
	}

	template<IsLoadable Resource, Enum Identifier>
	template<class Parameter>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string_view path, const Parameter& second_param)
	{
		Resource resource{};

		if (!resource.loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return resource;
	}

	template<IsLoadable Resource, Enum Identifier>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path)
	{
		ResourcePtr resource = std::make_unique<Resource>(Load(path));
		auto inserted = m_resources.try_emplace(id, std::move(resource));
		assert(inserted.second);
	}

	template<IsLoadable Resource, Enum Identifier>
	template<typename Parameter>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path, const Parameter& second_param)
	{
		ResourcePtr resource = std::make_unique<Resource>(Load(path, second_param));
		auto inserted = m_resources.try_emplace(id, std::move(resource));
		assert(inserted.second);
	}

	template<IsLoadable Resource, Enum Identifier>
	inline std::future<void> ResourceHolder<Resource, Identifier>::LoadAsync(const Identifier& id, const std::string& path)
	{
		return std::async(std::launch::async, 
			[this, id, path]() // create local copy of path because it will later be destroyed
			{
				ResourcePtr resource = std::make_unique<Resource>(Load(path));

				std::lock_guard lock(m_mutex); // guard race condition for m_resources

				auto inserted = m_resources.try_emplace(id, std::move(resource));
				assert(inserted.second);
			});
	}

	template<IsLoadable Resource, Enum Identifier>
	inline Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id)
	{
		const auto it = m_resources.find(id);

		if (it == m_resources.end())
			throw std::runtime_error("value could not be found");

		return *it->second;
	}

	template<IsLoadable Resource, Enum Identifier>
	inline const Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id) const
	{
		return const_cast<ResourceHolder<Resource, Identifier>&>(*this).Get(id);
	}

	using TextureHolder = ResourceHolder<sf::Texture, Texture::ID>;
	using FontHolder = ResourceHolder<sf::Font, Font::ID>;
}

