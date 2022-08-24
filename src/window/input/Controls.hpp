#pragma once

#include <FGE/Concepts.hpp>

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
		template<std::derived_from<InputHandler> T >
		auto& Get()
		{
			const auto& id = typeid(T);

			if (!m_controls.contains(id))
				throw std::runtime_error("T has not been added to map");

			return *static_cast<T*>(m_controls[id].get());
		}
		template<std::derived_from<InputHandler> T>
		const auto& Get() const
		{
			return const_cast<Controls*>(this)->Get();
		}

		////////////////////////////////////////////////////////////
		// Add the type T that derives InputHandler to the controls
		////////////////////////////////////////////////////////////
		template<std::derived_from<InputHandler> T, typename... Args>
		void Add(Args&&... args)
		{
			const auto& id = typeid(T);

			if (m_controls.contains(id))
				throw std::runtime_error("There is already an object defined");

			m_controls[id] = InputHandler::Ptr(new T(std::forward<Args>(args)...));
		}

		template<std::derived_from<InputHandler> T>
		void Remove()
		{
			const auto& id = typeid(T);

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
			case sf::Event::GainedFocus: m_focus = true; break;
			case sf::Event::LostFocus: m_focus = false; break;
			}

			for (auto& control : m_controls)
				control.second->HandleEvent(event);
		}

	private:
		InputHandler::Controls m_controls;
		bool m_focus{true}; // buttons/keys are not registered if not in focus
	};
}