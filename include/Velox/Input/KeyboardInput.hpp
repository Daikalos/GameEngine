#pragma once

#include "InputHandler.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the keyboard input, only supports one
	// keyboard at a time
	////////////////////////////////////////////////////////////
	class KeyboardInput : public InputHandler
	{
	public:
		void Update(const Time& time, bool focus) override
		{
			for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
			{
				m_previous_state[i] = m_current_state[i];
				m_current_state[i] = focus && m_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

				m_held_timer[i] = m_current_state[i] ?
					m_held_timer[i] + (m_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
			}
		}
		void HandleEvent(const sf::Event& event) override
		{

		}

	public:
		bool Held(sf::Keyboard::Key key) const
		{
			return m_current_state[key] && m_held_timer[key] >= m_held_threshold;
		}
		bool Pressed(sf::Keyboard::Key key) const
		{
			return m_current_state[key] && !m_previous_state[key];
		}
		bool Released(sf::Keyboard::Key key) const
		{
			return !Pressed(key);
		}

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount]	= {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount]	= {false};
		float	m_held_timer		[sf::Keyboard::KeyCount]	= {0.0f};
	};
}