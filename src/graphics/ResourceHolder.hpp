#pragma once

#include <SFML/Graphics.hpp>

#include <future>
#include <memory>
#include <unordered_map>
#include <string_view>

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

		Resource Load(const std::string_view path);

		template <class Parameter>
		Resource Load(const std::string_view path, const Parameter& second_param);

		void Load(const Identifier& id, const std::string_view path);
		//auto LoadAsync(const Identifier& id, const std::string& path);

		template <class Parameter>
		void Load(const Identifier& id, const std::string_view path, const Parameter& second_param);

		Resource& Get(const Identifier& id);
		const Resource& Get(const Identifier& id) const;

	private:
		std::unordered_map<Identifier, ResourcePtr> m_resources;
	};

	template<class Resource, class Identifier>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string_view path)
	{
		Resource resource;

		if (!resource.loadFromFile(path))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return std::move(resource);
	}

	template<class Resource, class Identifier>
	template<class Parameter>
	inline Resource ResourceHolder<Resource, Identifier>::Load(const std::string_view path, const Parameter& second_param)
	{
		Resource resource;

		if (!resource.loadFromFile(path, second_param))
			throw std::runtime_error("resource does not exist at " + std::string(path));

		return std::move(resource);
	}

	template<typename Resource, typename Identifier>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path)
	{
		ResourcePtr resource = ResourcePtr(new Resource(Load(path)));
		auto inserted = m_resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	//template<class Resource, class Identifier>
	//inline auto ResourceHolder<Resource, Identifier>::LoadAsync(const Identifier& id, const std::string& path)
	//{
	//	return std::async(std::launch::async, &ResourceHolder<Resource, Identifier>::Load, std::ref(id), std::ref(path));
	//}

	template<typename Resource, typename Identifier>
	template<typename Parameter>
	inline void ResourceHolder<Resource, Identifier>::Load(const Identifier& id, const std::string_view path, const Parameter& second_param)
	{
		ResourcePtr resource = ResourcePtr(new Resource(Load(path, second_param)));
		auto inserted = m_resources.insert(std::make_pair(id, std::move(resource)));
		assert(inserted.second);
	}

	template<typename Resource, typename Identifier>
	inline Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id)
	{
		auto it = m_resources.find(id);
		assert(it != m_resources.end());

		return *it->second.get();
	}

	template<typename Resource, typename Identifier>
	inline const Resource& ResourceHolder<Resource, Identifier>::Get(const Identifier& id) const
	{
		return const_cast<ResourceHolder<Resource, Identifier>*>(this)->Get(id);
	}

	using TextureHolder = ResourceHolder<sf::Texture, Texture::ID>;
	using FontHolder = ResourceHolder<sf::Font, Font::ID>;
}

