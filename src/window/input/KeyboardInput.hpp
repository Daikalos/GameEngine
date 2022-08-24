#pragma once

#include <unordered_map>

#include "InputHandler.hpp"

namespace fge
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
				m_previous_key_state[i] = m_current_key_state[i];
				m_current_key_state[i] = focus && m_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

				m_key_held_timer[i] = m_current_key_state[i] ?
					m_key_held_timer[i] + (m_key_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
			}
		}
		void HandleEvent(const sf::Event& event) override
		{

		}

	public:
		bool Held(const sf::Keyboard::Key& key) const
		{
			return m_current_key_state[key] && m_key_held_timer[key] >= m_held_threshold;
		}
		bool Pressed(const sf::Keyboard::Key& key) const
		{
			return m_current_key_state[key] && !m_previous_key_state[key];
		}
		bool Released(const sf::Keyboard::Key& key) const
		{
			return !Pressed(key);
		}

	private:
		bool	m_current_key_state		[sf::Keyboard::KeyCount]	= {false};
		bool	m_previous_key_state	[sf::Keyboard::KeyCount]	= {false};
		float	m_key_held_timer		[sf::Keyboard::KeyCount]	= {0.0f};
	};
}