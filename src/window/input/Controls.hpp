#pragma once

#include "../../utilities/NonCopyable.h"
#include "../Window.h"
#include "InputHandler.hpp"

namespace fge
{
	////////////////////////////////////////////////////////////
	// Neat wrapper around controls to prevent having to
	// cast all the time to access each controller. Allows for
	// adding your own controls.
	////////////////////////////////////////////////////////////
	class Controls final : NonCopyable
	{
	public:
		template<class T, typename std::enable_if_t<std::is_base_of_v<InputHandler, T>, bool> = true>
		T& Get()
		{
			const std::type_info& id = typeid(T);

			if (!m_controls.contains(id))
				throw std::runtime_error("T has not been added to map");

			return *static_cast<T*>(m_controls[id].get());
		}
		template<class T, typename std::enable_if_t<std::is_base_of_v<InputHandler, T>, bool> = true>
		const T& Get() const
		{
			return const_cast<Controls*>(this)->Get();
		}

		////////////////////////////////////////////////////////////
		// Add the type T that derives InputHandler to the controls
		////////////////////////////////////////////////////////////
		template<class T, typename... Args, typename std::enable_if_t<std::is_base_of_v<InputHandler, T>, bool> = true>
		void Add(Args&&... args)
		{
			const std::type_info& id = typeid(T);

			if (m_controls.contains(id))
				throw std::runtime_error("There is already an object defined");

			m_controls[id] = InputHandler::Ptr(new T(std::forward<Args>(args)...));
		}

		template<class T, typename std::enable_if_t<std::is_base_of_v<InputHandler, T>, bool> = true>
		void Remove()
		{
			const std::type_info& id = typeid(T);

			if (!m_controls.contains(id))
				throw std::runtime_error("The object of type T does not exist");

			m_controls.erase(id);
		}

		void Update(const Time& time)
		{
			for (auto& control : m_controls)
				control.second->Update(time, m_focus);
		}
		void HandleEvent(const sf::Event& event)
		{
			switch (event.type)
			{
			case sf::Event::GainedFocus:
				m_focus = true;
				break;
			case sf::Event::LostFocus:
				m_focus = false;
				break;
			}

			for (auto& control : m_controls)
				control.second->HandleEvent(event);
		}

	private:
		InputHandler::Controls m_controls;
		bool m_focus{true}; // buttons/keys are not registered if not in focus
	};
}