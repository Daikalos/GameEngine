#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include "Systems/AnchorSystem.h"
#include "Systems/ObjectSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/RelationSystem.h"
#include "Systems/TransformSystem.h"

#include <Velox/Window/Window.h>
#include <Velox/Input.hpp>
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
		std::pair<S*, bool> AddSystem(Args&&... args) requires std::constructible_from<S, Args...>;

		template<std::derived_from<ISystemObject> S>
		void RemoveSystem();

		template<std::derived_from<ISystemObject> S>
		bool HasSystem() const;

	public:
		VELOX_API const ControlMap& GetControls() const noexcept;
		VELOX_API ControlMap& GetControls() noexcept;

	public:
		VELOX_API void RemoveSystem(LayerType id);
		VELOX_API bool HasSystem(LayerType id) const;

	public:
		VELOX_API void Update();
		VELOX_API void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	private:
		EntityAdmin		m_entity_admin;

		WorldSystems	m_systems;
		SortedSystems	m_sorted_systems;

		ControlMap		m_controls;
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
	inline std::pair<S*, bool> World::AddSystem(Args&&... args) requires std::constructible_from<S, Args...>
	{
		if (HasSystem<S>()) // don't add if already exists
			return { nullptr, false };

		ISystemObject::Ptr system = std::make_shared<S>(std::forward<Args>(args)...);

		m_systems[typeid(S)] = system;
		m_sorted_systems[system->GetID()] = std::make_pair(system, system.get());

		return { static_cast<S*>(system.get()), true };
	}

	template<std::derived_from<ISystemObject> S>
	inline void World::RemoveSystem()
	{
		m_systems.erase(typeid(S));
		
		m_sorted_systems.erase(std::find_if(m_sorted_systems.begin(), m_sorted_systems.end(), // find system that was removed
			[](const auto& item)
			{
				return item.second.first.expired();
			})); // and erase it from list
	}

	template<std::derived_from<ISystemObject> S>
	inline bool World::HasSystem() const
	{
		return m_systems.contains(typeid(S));
	}
}