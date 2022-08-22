#pragma once

#include <unordered_map>

#include "InputHandler.hpp"

namespace fge
{
	template<class B>
	class MouseHandler : public InputHandler
	{
	public:
		void Update(const Time& time) override
		{
			m_scroll_delta = 0.0f;

			for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
			{
				m_previous_button_state[i] = m_current_button_state[i];
				m_current_button_state[i] = m_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

				m_button_held_timer[i] = m_current_button_state[i] ?
					m_button_held_timer[i] + (m_button_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
			}
		}
		void HandleEvent(const sf::Event& event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseWheelScrolled:
				m_scroll_delta = event.mouseWheelScroll.delta;
				break;
			}
		}

	public:
		bool GetScrollUp() const noexcept { return m_scroll_delta > 0; }
		bool GetScrollDown() const noexcept { return m_scroll_delta < 0; }

		bool GetHeld(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && m_button_held_timer[button] >= m_held_threshold;
		}
		bool GetPressed(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && !m_previous_button_state[button];
		}
		bool GetReleased(const sf::Mouse::Button& button) const
		{
			return !GetPressed(button);
		}

		bool GetHeld(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist"); // note: only prints value and not name

			return GetHeld(m_button_bindings.at(button));
		}
		bool GetPressed(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist");

			return GetPressed(m_button_bindings.at(button));
		}
		bool GetReleased(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist");

			return GetReleased(m_button_bindings.at(button));
		}

		void SetBinding(const B& name, const sf::Mouse::Button& button)
		{
			m_button_bindings[name] = button;
		}
		void RemoveBinding(const B& name)
		{
			if (!m_button_bindings.contains(name))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(name)) + "] does not exist");

			m_button_bindings.erase(name);
		}

	private:
		float	m_scroll_delta		{0.0f};
		float	m_mouse_sensitivity	{1.0f}; // TODO: implement

		bool	m_current_button_state[sf::Mouse::ButtonCount]	= {false};
		bool	m_previous_button_state[sf::Mouse::ButtonCount] = {false};
		float	m_button_held_timer[sf::Mouse::ButtonCount]		= {0.0f};

		std::unordered_map<B, sf::Mouse::Button> m_button_bindings;	// bindings for buttons
	};

	using DefMouse = typename MouseHandler<bn::Button>;
}