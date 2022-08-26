#pragma once

#include <SFML/Window/Cursor.hpp>

#include "InputHandler.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the mouse input, only supports one mouse
	// at a time. 
	////////////////////////////////////////////////////////////
	class MouseInput : public InputHandler
	{
	public:
		void Update(const Time& time, bool focus) override
		{
			m_scroll_delta = 0.0f;

			for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
			{
				m_previous_button_state[i] = m_current_button_state[i];
				m_current_button_state[i] = focus && m_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

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
		bool ScrollUp() const noexcept { return m_scroll_delta > m_scroll_threshold; }
		bool ScrollDown() const noexcept { return m_scroll_delta < -m_scroll_threshold; }

		bool Held(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && m_button_held_timer[button] >= m_held_threshold;
		}
		bool Pressed(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && !m_previous_button_state[button];
		}
		bool Released(const sf::Mouse::Button& button) const
		{
			return !Pressed(button);
		}

	private:
		float	m_scroll_delta			{0.0f};
		float	m_scroll_threshold		{0.01f}; // threshold before scroll is considered for up/down

		bool	m_current_button_state	[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_button_state	[sf::Mouse::ButtonCount] = {false};
		float	m_button_held_timer		[sf::Mouse::ButtonCount] = {0.0f};
	};
}