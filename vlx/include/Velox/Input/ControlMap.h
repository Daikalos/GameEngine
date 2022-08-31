#pragma once

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Neat wrapper around controls to prevent having to
	// cast all the time to access each controller. Allows for
	// adding your own controls.
	////////////////////////////////////////////////////////////
	class ControlMap final : private NonCopyable
	{
	public:
		template<std::derived_from<InputHandler> T>
		auto& Get();
		template<std::derived_from<InputHandler> T>
		const auto& Get() const;

		////////////////////////////////////////////////////////////
		// Add the type T that derives InputHandler to the controls,
		// pass valid args if the constructor has parameters
		////////////////////////////////////////////////////////////
		template<std::derived_from<InputHandler> T, typename... Args>
		void Add(Args&&... args) requires std::constructible_from<T, Args...>;

		template<std::derived_from<InputHandler> T>
		void Remove();

		template<std::derived_from<InputHandler> T>
		bool Exists() const;

		VELOX_API void UpdateAll(const Time& time, const bool focus);
		VELOX_API void HandleEventAll(const sf::Event& event);

	private:
		InputHandler::ControlMap m_controls;
	};

	template<std::derived_from<InputHandler> T>
	inline auto& ControlMap::Get()
	{
		return *static_cast<T*>(m_controls.at(typeid(T)).get());
	}

	template<std::derived_from<InputHandler> T>
	inline const auto& ControlMap::Get() const
	{
		return const_cast<ControlMap*>(this)->Get<T>();
	}

	template<std::derived_from<InputHandler> T, typename... Args>
	inline void ControlMap::Add(Args&&... args) requires std::constructible_from<T, Args...>
	{
		m_controls[typeid(T)] = InputHandler::Ptr(new T(std::forward<Args>(args)...));
	}

	template<std::derived_from<InputHandler> T>
	inline void ControlMap::Remove()
	{
		m_controls.erase(typeid(T));
	}

	template<std::derived_from<InputHandler> T>
	inline bool ControlMap::Exists() const
	{
		return m_controls.contains(typeid(T));
	}
}