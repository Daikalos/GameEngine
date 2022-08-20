#pragma once

#include <SFML/Graphics.hpp>

#include <future>
#include <memory>
#include <unordered_map>

#include "../utilities/NonCopyable.h"
#include "Resources.h"

namespace fge
{
	template <class Resource, class Identifier>
	class ResourceHolder : private NonCopyable
	{
	public:
		using ResourcePtr = typename std::unique_ptr<Resource>;

	public:
		ResourceHolder() { }
		~ResourceHolder() { }

		Resource Load(const std::string& path);

		template <class Parameter>
		Resource Load(const std::string& path, const Parameter& second_param);

		void Load(const Identifier& id, const std::string& path);
		//auto LoadAsync(const Identifier& id, const std::string& path);

		template <class Parameter>
		void Load(const Identifier& id, const std::string& path, const Parameter& second_param);

		Resource& Get(const Identifier& id);
		const Resource& Get(const Identifier& id) const;

	private:
		std::unordered_map<Identifier, ResourcePtr> m_resources;
	};

	template<class Resource, class Identifier>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string& path)
	{
		Resource resource;

		if (!resource.loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + path);

		return std::move(resource);
	}

	template<class Resource, class Identifier>
	template<class Parameter>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string& path, const Parameter& second_param)
	{
		Resource resource;

		if (!resource.loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + path);

		return std::move(resource);
	}

	template<typename Resource, typename Identifier>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string& path)
	{
		ResourcePtr resource = ResourcePtr(new Resource(Load(path)));
		auto inserted = m_resources.insert(std::make_pair(id, resource));
		assert(inserted.second);
	}

	//template<class Resource, class Identifier>
	//inline auto ResourceHolder<Resource, Identifier>::LoadAsync(const Identifier& id, const std::string& path)
	//{
	//	return std::async(std::launch::async, &ResourceHolder<Resource, Identifier>::Load, std::ref(id), std::ref(path));
	//}

	template<typename Resource, typename Identifier>
	template<typename Parameter>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string& path, const Parameter& second_param)
	{
		ResourcePtr resource = ResourcePtr(new Resource(Load(path, second_param)));
		auto inserted = m_resources.insert(std::make_pair(id, resource));
		assert(inserted.second);
	}

	template<typename Resource, typename Identifier>
	inline Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id)
	{
		return Get(id);
	}

	template<typename Resource, typename Identifier>
	inline const Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id) const
	{
		auto it = m_resources.find(id);
		assert(it != m_resources.end());

		return *it->second.get();
	}

	using TextureHolder = ResourceHolder<sf::Texture, Texture::ID>;
	using FontHolder = ResourceHolder<sf::Font, Font::ID>;
}

