#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include <Velox/Input.hpp>
#include <Velox/Window.hpp>
#include <Velox/Graphics.hpp>
#include <Velox/ECS.hpp>
#include <Velox/UI.hpp>
#include <Velox/Physics.hpp>
#include <Velox/Config.hpp>

#include "EngineBinds.h"
#include "ObjectSystem.h"
#include "ObjectTypes.h"
#include "StateStack.h"

namespace vlx
{
	///	Collection of all important systems and objects that the world depends on
	/// 
	class World : private NonCopyable
	{
	private:
		using SystemIDType = std::size_t;

		using WorldSystems = std::multimap<LayerType, SystemAction::Ptr>;
		using SystemTable = std::unordered_map<SystemIDType, SystemAction*>;

	public:
		VELOX_API World(std::string name);

	public:
		NODISC VELOX_API const InputHolder& GetInputs() const noexcept;
		NODISC VELOX_API InputHolder& GetInputs() noexcept;

		NODISC VELOX_API const Window& GetWindow() const noexcept;
		NODISC VELOX_API Window& GetWindow() noexcept;

		NODISC VELOX_API const Camera& GetCamera() const noexcept;
		NODISC VELOX_API Camera& GetCamera() noexcept;

		NODISC VELOX_API const TextureHolder& GetTextureHolder() const noexcept;
		NODISC VELOX_API TextureHolder& GetTextureHolder() noexcept;

		NODISC VELOX_API const FontHolder& GetFontHolder() const noexcept;
		NODISC VELOX_API FontHolder& GetFontHolder() noexcept;

		NODISC VELOX_API const Time& GetTime() const noexcept;
		NODISC VELOX_API Time& GetTime() noexcept;

		NODISC VELOX_API const StateStack& GetStateStack() const noexcept;
		NODISC VELOX_API StateStack& GetStateStack() noexcept;

		NODISC VELOX_API const EntityAdmin& GetEntityAdmin() const noexcept;
		NODISC VELOX_API EntityAdmin& GetEntityAdmin() noexcept;

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
		VELOX_API void Run();

	private:
		VELOX_API void Start();
		VELOX_API void PreUpdate();
		VELOX_API void Update();
		VELOX_API void FixedUpdate();
		VELOX_API void PostUpdate();

		VELOX_API void ProcessEvents();
		VELOX_API void Draw();

	private:
		Time			m_time;
		Window			m_window;

		InputHolder		m_inputs;
		EngineKeyboard	m_keyboard;
		EngineMouse		m_mouse;

		TextureHolder	m_textures;
		FontHolder		m_fonts;

		Camera			m_camera;

		EntityAdmin		m_entity_admin;

		WorldSystems	m_systems;
		SystemTable		m_system_table;

		StateStack		m_state_stack;

		bool			m_shutdown {false};
	};

	template<std::derived_from<SystemAction> S>
	inline const S& World::GetSystem() const
	{
		constexpr auto system_id = id::Type<S>::ID();
		return *static_cast<S*>(m_system_table.at(system_id));
	}

	template<std::derived_from<SystemAction> S>
	inline S& World::GetSystem()
	{
		return const_cast<S&>(std::as_const(*this).GetSystem<S>());
	}

	template<std::derived_from<SystemAction> S, typename... Args>
	inline std::optional<S*> World::AddSystem(Args&&... args) requires std::constructible_from<S, Args...>
	{
		if (HasSystem<S>()) // don't add if it already exists
			return std::nullopt;

		constexpr auto system_id = id::Type<S>::ID();

		SystemAction::Ptr ptr = std::make_unique<S>(std::forward<Args>(args)...);

		const auto [lit, inserted] = m_system_table.emplace(system_id, ptr.get());
		assert(inserted);

		m_systems.emplace(ptr->GetLayer(), std::move(ptr));

		return static_cast<S*>(lit->second);
	}

	template<std::derived_from<SystemAction> S>
	inline void World::RemoveSystem()
	{
		constexpr auto system_id = id::Type<S>::ID();

		const auto lit = m_system_table.find(system_id);
		if (lit == m_system_table.end() || lit->second->IsRequired()) // cant remove required system
			return;

		const auto [begin, end] = m_systems.equal_range(lit->second->GetLayer());

		if (begin == m_systems.end() || end == m_systems.end())
			return;

		for (auto it = begin; it != end; ++it)
		{
			if (it->second.get() == lit->second)
			{
				m_systems.erase(it);
				break;
			}
		}

		m_system_table.erase(lit);
	}

	template<std::derived_from<SystemAction> S>
	inline bool World::HasSystem() const
	{
		constexpr auto system_id = id::Type<S>::ID();
		return m_system_table.contains(system_id);
	}
}