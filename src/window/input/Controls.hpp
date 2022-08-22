#pragma once

#include "../../utilities/NonCopyable.h"
#include "InputHandler.hpp"

namespace fge
{
	////////////////////////////////////////////////////////////
	// Neat wrapper around controls to prevent having to
	// cast all the time to access each controller
	////////////////////////////////////////////////////////////
	class Controls final : NonCopyable
	{
	public:
		template<class T> // unfortunately we cannot check if T derives from InputHandler, will have to create custom trait for that
		T& Get()
		{
			if (!m_controls.contains(typeid(T)))
				throw std::runtime_error("T has not been added to map");

			return *static_cast<T*>(m_controls[typeid(T)].get());
		}
		template<class T>
		const T& Get() const
		{
			return const_cast<Controls*>(this)->Get();
		}

		template<class T, typename... Args>
		void Add(Args&&... args)
		{
			if (m_controls.contains(typeid(T)))
				throw std::runtime_error("There is already an object defined");

			m_controls[typeid(T)] = InputHandler::Ptr(new T(std::forward<Args>(args)...));
		}

		void Update(const Time& time)
		{
			for (auto& control : m_controls)
				control.second->Update(time);
		}
		void HandleEvent(const sf::Event& event)
		{
			for (auto& control : m_controls)
				control.second->HandleEvent(event);
		}

	private:
		InputHandler::Controls m_controls;
	};
}