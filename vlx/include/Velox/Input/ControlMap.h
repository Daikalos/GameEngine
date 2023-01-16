#pragma once

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	/// <summary>
	///		Neat wrapper around controls to prevent having to cast all the time 
	///		to access each controller. Allows for adding your own controls.
	/// </summary>
	class ControlMap final : private NonCopyable
	{
	public:
		template<std::derived_from<InputHandler> T>
		const T& Get() const;

		template<std::derived_from<InputHandler> T>
		T& Get();

	public:
		/// <summary>
		///		Add the type T that derives InputHandler to the controls,
		///		pass valid args if the constructor has parameters
		/// </summary>
		template<std::derived_from<InputHandler> T, typename... Args>
		void Add(Args&&... args) requires std::constructible_from<T, Args...>;

		template<std::derived_from<InputHandler> T>
		void Remove();

		template<std::derived_from<InputHandler> T>
		bool Exists() const;

	public:
		VELOX_API void UpdateAll(const Time& time, const bool focus);
		VELOX_API void HandleEventAll(const sf::Event& event);

	private:
		InputHandler::ControlMap m_controls;
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