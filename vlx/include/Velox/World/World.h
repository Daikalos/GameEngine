#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include "Systems/AnchorSystem.h"
#include "Systems/ObjectSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/RelationSystem.h"
#include "Systems/TransformSystem.h"

#include <Velox/Window/Window.h>
#include <Velox/Window/Camera.h>

#include <Velox/Utilities.hpp>
#include <Velox/Input.hpp>
#include <Velox/Graphics.hpp>
#include <Velox/Config.hpp>

#include <Velox/Scene/StateStack.h>

#include <typeindex>
#include <map>
#include <unordered_map>

namespace vlx
{
	/// <summary>
	///		Composite of all important systems and objects that the world runs on
	/// </summary>
	class World : private NonCopyable
	{
	private:
		using SystemTracker = std::pair<std::weak_ptr<ISystemObject>, ISystemObject*>;

		using WorldSystems	= std::unordered_map<std::type_index, ISystemObject::Ptr>;
		using SortedSystems = std::map<LayerType, SystemTracker>;

	public:
		VELOX_API World(const std::string_view name);

	public:
		VELOX_API [[nodiscard]] const ControlMap& GetControls() const noexcept;
		VELOX_API [[nodiscard]] ControlMap& GetControls() noexcept;

		VELOX_API [[nodiscard]] const Window& GetWindow() const noexcept;
		VELOX_API [[nodiscard]] Window& GetWindow() noexcept;

		VELOX_API [[nodiscard]] const Camera& GetCamera() const noexcept;
		VELOX_API [[nodiscard]] Camera& GetCamera() noexcept;

		VELOX_API [[nodiscard]] const TextureHolder& GetTextureHolder() const noexcept;
		VELOX_API [[nodiscard]] TextureHolder& GetTextureHolder() noexcept;

		VELOX_API [[nodiscard]] const FontHolder& GetFontHolder() const noexcept;
		VELOX_API [[nodiscard]] FontHolder& GetFontHolder() noexcept;

		VELOX_API [[nodiscard]] const Time& GetTime() const noexcept;
		VELOX_API [[nodiscard]] Time& GetTime() noexcept;

		VELOX_API [[nodiscard]] const StateStack& GetStateStack() const noexcept;
		VELOX_API [[nodiscard]] StateStack& GetStateStack() noexcept;

		VELOX_API [[nodiscard]] const EntityAdmin& GetEntityAdmin() const noexcept;
		VELOX_API [[nodiscard]] EntityAdmin& GetEntityAdmin() noexcept;

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

		ControlMap		m_controls;
		Window			m_window;
		Camera			m_camera;

		TextureHolder	m_textures;
		FontHolder		m_fonts;

		Time			m_time;
		StateStack		m_state_stack;
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