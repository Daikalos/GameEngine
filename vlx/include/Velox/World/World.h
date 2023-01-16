#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include "Systems/AnchorSystem.h"
#include "Systems/ObjectSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/RelationSystem.h"
#include "Systems/TransformSystem.h"

#include <Velox/Window/Window.h>

#include <Velox/Config.hpp>

#include <typeindex>
#include <map>
#include <unordered_map>

namespace vlx
{
	/// <summary>
	///		Composite of all important systems that the world runs on
	/// </summary>
	class World : public sf::Drawable
	{
	private:
		using SystemTracker = std::pair<std::weak_ptr<ISystemObject>, ISystemObject*>;

		using WorldSystems	= std::unordered_map<std::type_index, ISystemObject::Ptr>;
		using SortedSystems = std::map<LayerType, SystemTracker>;

	public:
		VELOX_API World(const Window& window, const Time& time);

	public:
		VELOX_API [[nodiscard]] EntityAdmin&		GetEntityAdmin() noexcept;
		VELOX_API [[nodiscard]] const EntityAdmin&	GetEntityAdmin() const noexcept;

	public:
		template<std::derived_from<ISystemObject> S>
		[[nodiscard]] const S& Get() const;

		template<std::derived_from<ISystemObject> S>
		[[nodiscard]] S& Get();

		template<std::derived_from<ISystemObject> S, typename... Args>
		S& Add(Args&&... args) requires std::constructible_from<S, Args...>;

		template<std::derived_from<ISystemObject> S>
		void Remove();

		template<std::derived_from<ISystemObject> S>
		bool Has() const;

	public:
		VELOX_API void Update();
		VELOX_API void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	private:
		EntityAdmin		m_entity_admin;

		WorldSystems	m_systems;
		SortedSystems	m_sorted_systems;
	};

	template<std::derived_from<ISystemObject> S>
	inline const S& World::Get() const
	{
		return *static_cast<S*>(m_systems.at(typeid(S)).get());
	}

	template<std::derived_from<ISystemObject> S>
	inline S& World::Get()
	{
		return const_cast<S&>(std::as_const(*this).Get<S>());
	}

	template<std::derived_from<ISystemObject> S, typename... Args>
	inline S& World::Add(Args&&... args) requires std::constructible_from<S, Args...>
	{
		ISystemObject::Ptr system = std::make_shared<S>(std::forward<Args>(args)...);

		m_systems[typeid(S)] = system;
		m_sorted_systems[system->GetID()] = std::make_pair(system, system.get());

		return static_cast<S&>(*system);
	}

	template<std::derived_from<ISystemObject> S>
	inline void World::Remove()
	{
		m_systems.erase(typeid(S));
		
		const auto it = std::find_if(m_sorted_systems.begin(), m_sorted_systems.end(),
			[](const auto& item)
			{
				return item.second.first.expired();
			});

		m_sorted_systems.erase(it);
	}

	template<std::derived_from<ISystemObject> S>
	inline bool World::Has() const
	{
		return m_systems.contains(typeid(S));
	}
}