#pragma once

#include <Velox/Concepts.hpp>

#include "../utilities/NonCopyable.h"
#include "../utilities/ContainerUtilities.h"

#include "InputHandler.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Neat wrapper around controls to prevent having to
	// cast all the time to access each controller. Allows for
	// adding your own controls.
	////////////////////////////////////////////////////////////
	class Controls final : NonCopyable
	{
	public:
		template<std::derived_from<InputHandler> T>
		auto& Get()
		{
			return *static_cast<T*>(m_controls.at(typeid(T)).get());
		}
		template<std::derived_from<InputHandler> T>
		const auto& Get() const
		{
			return const_cast<Controls*>(this)->Get();
		}

		////////////////////////////////////////////////////////////
		// Add the type T that derives InputHandler to the controls,
		// pass valid args if the constructor has parameters
		////////////////////////////////////////////////////////////
		template<std::derived_from<InputHandler> T, typename... Args>
		void Add(Args&&... args) requires std::constructible_from<T, Args...>
		{
			const auto& id = typeid(T);

			if (m_controls.contains(id))
				throw std::runtime_error("There is already an object defined");

			auto ptr = InputHandler::Ptr(new T(std::forward<Args>(args)...));

			m_controls_list.push_back(ptr.get());
			m_controls[id] = std::move(ptr);
		}

		template<std::derived_from<InputHandler> T>
		void Remove()
		{
			const auto& id = typeid(T);

			if (!m_controls.contains(id))
				throw std::runtime_error("The object of type T does not exist in map");

			bool result = cu::Erase(m_controls_list, &Get<T>());

			if (!result)
				throw std::runtime_error("The object of type T does not exist in list");

			m_controls.erase(id);
		}

		template<std::derived_from<InputHandler> T>
		bool Exists() const
		{
			return m_controls.contains(typeid(T));
		}

		void Update(const Time& time)
		{
			for (auto& control : m_controls_list)
				control->Update(time, m_focus);
		}
		void HandleEvent(const sf::Event& event)
		{
			switch (event.type)
			{
			case sf::Event::GainedFocus: m_focus = true; break;
			case sf::Event::LostFocus: m_focus = false; break;
			}

			for (auto& control : m_controls_list)
				control->HandleEvent(event);
		}

	private:
		InputHandler::Controls m_controls;
		std::vector<InputHandler*> m_controls_list; // no idea if this improves performance, will have to do a comparison
		bool m_focus{true}; // buttons/keys are not registered if not in focus
	};
}