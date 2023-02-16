#pragma once

#include <typeindex>
#include <map>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Input.hpp>
#include <Velox/Graphics.hpp>
#include <Velox/Config.hpp>
#include <Velox/Window.hpp>

#include <Velox/Scene/StateStack.h>

#include "Systems/AnchorSystem.h"
#include "Systems/ObjectSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/RelationSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/GUISystem.h"
#include "Systems/CullingSystem.h"

#include "EngineBinds.h"

namespace vlx
{
	/// <summary>
	///		Composite of all important systems and objects that the world runs on
	/// </summary>
	class World : private NonCopyable
	{
	private:
		using SystemTracker = std::pair<std::weak_ptr<SystemObject>, SystemObject*>;

		using WorldSystems	= std::unordered_map<std::type_index, SystemObject::Ptr>;
		using SortedSystems = std::map<LayerType, SystemTracker>;

	public:
		VELOX_API World(const std::string_view name);

	public:
		VELOX_API NODISC const ControlMap& GetControls() const noexcept;
		VELOX_API NODISC ControlMap& GetControls() noexcept;

		VELOX_API NODISC const Window& GetWindow() const noexcept;
		VELOX_API NODISC Window& GetWindow() noexcept;

		VELOX_API NODISC const Camera& GetCamera() const noexcept;
		VELOX_API NODISC Camera& GetCamera() noexcept;

		VELOX_API NODISC const TextureHolder& GetTextureHolder() const noexcept;
		VELOX_API NODISC TextureHolder& GetTextureHolder() noexcept;

		VELOX_API NODISC const FontHolder& GetFontHolder() const noexcept;
		VELOX_API NODISC FontHolder& GetFontHolder() noexcept;

		VELOX_API NODISC const Time& GetTime() const noexcept;
		VELOX_API NODISC Time& GetTime() noexcept;

		VELOX_API NODISC const StateStack& GetStateStack() const noexcept;
		VELOX_API NODISC StateStack& GetStateStack() noexcept;

		VELOX_API NODISC const EntityAdmin& GetEntityAdmin() const noexcept;
		VELOX_API NODISC EntityAdmin& GetEntityAdmin() noexcept;

	public:
		template<std::derived_from<SystemObject> S>
		NODISC const S& GetSystem() const;

		template<std::derived_from<SystemObject> S>
		NODISC S& GetSystem();

		template<std::derived_from<SystemObject> S, typename... Args>
		std::optional<S*> AddSystem(Args&&... args) requires std::constructible_from<S, Args...>;

		template<std::derived_from<SystemObject> S>
		void RemoveSystem();

		template<std::derived_from<SystemObject> S>
		bool HasSystem() const;

	public:
		VELOX_API void RemoveSystem(const LayerType id);
		VELOX_API bool HasSystem(const LayerType id) const;

	public:
		VELOX_API void Run();

	private:
		VELOX_API void PreUpdate();
		VELOX_API void Update();
		VELOX_API void FixedUpdate();
		VELOX_API void PostUpdate();

		VELOX_API void ProcessEvents();
		VELOX_API void Draw();

	private:
		EntityAdmin		m_entity_admin;
		WorldSystems	m_systems;
		SortedSystems	m_sorted_systems;

		RenderSystem	m_render_system; // obligatory system
		// physics system

		ControlMap		m_controls;
		Window			m_window;
		Camera			m_camera;

		TextureHolder	m_textures;
		FontHolder		m_fonts;

		Time			m_time;
		StateStack		m_state_stack;
	};

	template<std::derived_from<SystemObject> S>
	inline const S& World::GetSystem() const
	{
		return *static_cast<S*>(m_systems.at(typeid(S)).get());
	}

	template<std::derived_from<SystemObject> S>
	inline S& World::GetSystem()
	{
		return const_cast<S&>(std::as_const(*this).GetSystem<S>());
	}

	template<std::derived_from<SystemObject> S, typename... Args>
	inline std::optional<S*> World::AddSystem(Args&&... args) requires std::constructible_from<S, Args...>
	{
		if (HasSystem<S>()) // don't add if already exists
			return std::nullopt;

		SystemObject::Ptr system = std::make_shared<S>(std::forward<Args>(args)...);

		m_systems[typeid(S)] = system;
		m_sorted_systems[system->GetID()] = std::make_pair(system, system.get());

		return static_cast<S*>(system.get());
	}

	template<std::derived_from<SystemObject> S>
	inline void World::RemoveSystem()
	{
		m_systems.erase(typeid(S));

		m_sorted_systems.erase(std::find_if(m_sorted_systems.begin(), m_sorted_systems.end(), // find system that was removed
			[](const auto& item)
			{
				return item.second.first.expired();
			})); // and erase it from list
	}

	template<std::derived_from<SystemObject> S>
	inline bool World::HasSystem() const
	{
		return m_systems.contains(typeid(S));
	}
}