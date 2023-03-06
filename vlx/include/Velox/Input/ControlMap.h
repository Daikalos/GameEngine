#pragma once

#include <SFML/Window/Event.hpp>

#include <Velox/System/Time.h>
#include <Velox/Utility/NonCopyable.h>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	///	Neat wrapper around controls to prevent having to cast all the time 
	///	to access each controller. Allows for adding your own controls.
	/// 
	class ControlMap final : private NonCopyable
	{
	public:
		///	\returns Retrieves the InputHandler
		/// 
		template<std::derived_from<InputHandler> T>
		NODISC const T& Get() const;

		///	\returns Retrieves the InputHandler
		/// 
		template<std::derived_from<InputHandler> T>
		NODISC T& Get();

	public:
		///	Adds the InputHandler to the controls
		/// 
		/// \param Args: Optional constructor arguments
		/// 
		template<std::derived_from<InputHandler> T, typename... Args>
		void Add(Args&&... args) requires std::constructible_from<T, Args...>;


		///	Removes the InputHandler from the controls
		/// 
		template<std::derived_from<InputHandler> T>
		void Remove();

		///	\returns True if the InputHandler has been added to the controls, otherwise false
		/// 
		template<std::derived_from<InputHandler> T>
		NODISC bool Exists() const;

	public:
		VELOX_API void UpdateAll(const Time& time, const bool focus);
		VELOX_API void HandleEventAll(const sf::Event& event);

	private:
		InputHandler::ControlsMap m_controls;
	};

	template<std::derived_from<InputHandler> T>
	inline const T& ControlMap::Get() const
	{
		return *static_cast<T*>(m_controls.at(typeid(T)).get());
	}

	template<std::derived_from<InputHandler> T>
	inline T& ControlMap::Get()
	{
		return const_cast<T&>(std::as_const(*this).Get<T>());
	}

	template<std::derived_from<InputHandler> T, typename... Args>
	inline void ControlMap::Add(Args&&... args) requires std::constructible_from<T, Args...>
	{
		m_controls[typeid(T)] = std::make_unique<T>(std::forward<Args>(args)...);
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