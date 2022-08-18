#include "InputHandler.h"

using namespace fge;

InputHandler::InputHandler()
	: m_mouse_enabled(true), m_keyboard_enabled(true), m_joystick_enabled(false), m_held_threshold(0.1f)
{
#if KEYBOARDMOUSE_ENABLED
	m_scroll_delta = 0.0f;

	memset(m_current_button_state, false, sizeof(m_current_button_state));
	memset(m_previous_button_state, false, sizeof(m_previous_button_state));

	memset(m_current_key_state, false, sizeof(m_current_key_state));
	memset(m_previous_key_state, false, sizeof(m_previous_key_state));
#endif

#if JOYSTICK_ENABLED
	memset(m_current_button_joystick_state, false, sizeof(m_current_button_joystick_state));
	memset(m_previous_button_joystick_state, false, sizeof(m_previous_button_joystick_state));

	m_available_joysticks.rehash(sf::Joystick::Count);

	for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
		if (sf::Joystick::isConnected(i))
			m_available_joysticks.insert(i);
#endif
}

void InputHandler::Update(const Time& time)
{
#if KEYBOARDMOUSE_ENABLED
	m_scroll_delta = 0.0f;

	for (uint i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		m_previous_button_state[i] = m_current_button_state[i];
		m_current_button_state[i] = m_mouse_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

		m_button_held_timer[i] = m_current_button_state[i] ? 
			m_button_held_timer[i] + (m_button_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
	}

	for (uint i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		m_previous_key_state[i] = m_current_key_state[i];
		m_current_key_state[i] = m_keyboard_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

		m_key_held_timer[i] = m_current_key_state[i] ? 
			m_key_held_timer[i] + (m_key_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
	}
#endif

#if JOYSTICK_ENABLED
	for (const uint& i : m_available_joysticks)
	{
		for (uint j = 0; j < sf::Joystick::getButtonCount(i); ++j)
		{
			uint k = j + i * sf::Joystick::ButtonCount;

			m_previous_button_joystick_state[k] = m_current_button_joystick_state[k];
			m_current_button_joystick_state[k] = m_joystick_enabled && sf::Joystick::isButtonPressed(i, j);

			m_joystick_button_held_timer[k] = m_current_button_joystick_state[k] ?
				m_joystick_button_held_timer[k] + (m_joystick_button_held_timer[k] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
		}

		for (uint j = 0; j < sf::Joystick::AxisCount; ++j)
		{
			uint k = j + i * sf::Joystick::AxisCount;

			sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);
			m_joystick_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) : 0.0f;
		}
	}
#endif
}

void InputHandler::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
#if KEYBOARDMOUSE_ENABLED
	case sf::Event::MouseWheelScrolled:
		m_scroll_delta = event.mouseWheelScroll.delta;
		break;
#endif

#if JOYSTICK_ENABLED
	case sf::Event::JoystickConnected:
		m_available_joysticks.insert(event.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
		{
			for (uint i = 0; i < sf::Joystick::ButtonCount; ++i) // be sure to set all to default state
			{
				uint j = i + event.joystickConnect.joystickId * sf::Joystick::ButtonCount;

				m_previous_button_joystick_state[j] = false;
				m_current_button_joystick_state[j] = false;
				m_joystick_button_held_timer[j] = 0.0f;
			}

			for (uint i = 0; i < sf::Joystick::AxisCount; ++i)
			{
				uint j = i + event.joystickConnect.joystickId * sf::Joystick::AxisCount;

				m_joystick_axis[j] = 0.0f;
			}

			m_available_joysticks.erase(event.joystickConnect.joystickId);
		}
		break;
#endif
	}
}
