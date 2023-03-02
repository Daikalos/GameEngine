#pragma once

#include <typeindex>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Input.hpp>
#include <Velox/Graphics.hpp>
#include <Velox/Config.hpp>
#include <Velox/Window.hpp>

#include <Velox/Scene/StateStack.h>

#include "EngineBinds.h"

namespace vlx
{
	/// <summary>
	///		Composite of all important systems and objects that the world runs on
	/// </summary>
	class World : private NonCopyable
	{
	private:
		using WorldSystems	= std::unordered_map<std::type_index, SystemAction::Ptr>;
		using SortedSystems = std::vector<SystemAction*>;

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
		template<std::derived_from<SystemAction> S>
		NODISC const S& GetSystem() const;

		template<std::derived_from<SystemAction> S>
		NODISC S& GetSystem();

		template<std::derived_from<SystemAction> S, typename... Args>
		std::optional<S*> AddSystem(Args&&... args) requires std::constructible_from<S, Args...>;

		template<std::derived_from<SystemAction> S>
		void RemoveSystem();

		template<std::derived_from<SystemAction> S>
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

		Time			m_time;
		StateStack		m_state_stack;

		ControlMap		m_controls;
		Window			m_window;
		Camera			m_camera;

		TextureHolder	m_textures;
		FontHolder		m_fonts;

		WorldSystems	m_systems;
		SortedSystems	m_sorted_systems;
	};

	template<std::derived_from<SystemAction> S>
	inline const S& World::GetSystem() const
	{
		return *static_cast<S*>(m_systems.at(typeid(S)).get());
	}

	template<std::derived_from<SystemAction> S>
	inline S& World::GetSystem()
	{
		return const_cast<S&>(std::as_const(*this).GetSystem<S>());
	}

	template<std::derived_from<SystemAction> S, typename... Args>
	inline std::optional<S*> World::AddSystem(Args&&... args) requires std::constructible_from<S, Args...>
	{
		if (HasSystem<S>()) // don't add if already exists
			return std::nullopt;

		auto system = std::make_unique<S>(std::forward<Args>(args)...);
		auto system_ptr = system.get();

		m_systems[typeid(S)] = std::move(system);

		cu::InsertSorted(m_sorted_systems, system_ptr,
			[](const SystemAction* lhs, const SystemAction* rhs)
			{
				return lhs->GetID() < rhs->GetID();
			});

		return system_ptr;
	}

	template<std::derived_from<SystemAction> S>
	inline void World::RemoveSystem()
	{
		const auto it = m_systems.find(typeid(S));
		if (it == m_systems.end())
			return;

		if (it->second.get()->IsRequired()) // cant remove required system
			return;

		m_sorted_systems.erase(std::ranges::find_if(m_sorted_systems.begin(), m_sorted_systems.end(),
			[&it](const auto& item)
			{
				return item == it->second.get();
			}));

		m_systems.erase(it);
	}

	template<std::derived_from<SystemAction> S>
	inline bool World::HasSystem() const
	{
		return m_systems.contains(typeid(S));
	}
}