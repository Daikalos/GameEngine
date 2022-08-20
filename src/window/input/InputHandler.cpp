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

#if KEYBOARDMOUSE_ENABLED
bool InputHandler::GetScrollUp() const noexcept
{
	return m_scroll_delta > 0;
}

bool fge::InputHandler::GetScrollDown() const noexcept
{
	return m_scroll_delta < 0;
}

bool InputHandler::GetButtonHeld(const sf::Mouse::Button& button) const
{
	return m_current_button_state[button] && m_button_held_timer[button] >= m_held_threshold;
}
bool InputHandler::GetButtonPressed(const sf::Mouse::Button& button) const
{
	return m_current_button_state[button] && !m_previous_button_state[button];
}
bool InputHandler::GetButtonReleased(const sf::Mouse::Button& button) const
{
	return !GetButtonPressed(button);
}

bool InputHandler::GetButtonHeld(const Binds::Button& button) const
{
	assert(m_button_bindings.contains(button)); // should exist in the first place if trying to get
	return GetButtonHeld(m_button_bindings.at(button));
}
bool InputHandler::GetButtonPressed(const Binds::Button& button) const
{
	assert(m_button_bindings.contains(button));
	return GetButtonPressed(m_button_bindings.at(button));
}
bool InputHandler::GetButtonReleased(const Binds::Button& button) const
{
	assert(m_button_bindings.contains(button));
	return GetButtonReleased(m_button_bindings.at(button));
}

bool InputHandler::GetKeyHeld(const sf::Keyboard::Key& key) const
{
	return m_current_key_state[key] && m_key_held_timer[key] >= m_held_threshold;
}
bool InputHandler::GetKeyPressed(const sf::Keyboard::Key& key) const
{
	return m_current_key_state[key] && !m_previous_key_state[key];
}
bool InputHandler::GetKeyReleased(const sf::Keyboard::Key& key) const
{
	return !GetKeyPressed(key);
}

bool InputHandler::GetKeyHeld(const Binds::Key& key) const
{
	assert(m_key_bindings.contains(key));
	return GetKeyHeld(m_key_bindings.at(key));
}
bool InputHandler::GetKeyPressed(const Binds::Key& key) const
{
	assert(m_key_bindings.contains(key));
	return GetKeyPressed(m_key_bindings.at(key));
}
bool InputHandler::GetKeyReleased(const Binds::Key& key) const
{
	assert(m_key_bindings.contains(key));
	return GetKeyReleased(m_key_bindings.at(key));
}

void InputHandler::SetButtonBinding(const Binds::Button& name, const sf::Mouse::Button& button)
{
	m_button_bindings[name] = button;
}
void InputHandler::SetKeyBinding(const Binds::Key& name, const sf::Keyboard::Key& key)
{
	m_key_bindings[name] = key;
}

void InputHandler::RemoveButtonBinding(const Binds::Button& name)
{
	assert(m_button_bindings.contains(name)); // should not be removed if it does not exist in the first place
	m_button_bindings.erase(name);
}
void InputHandler::RemoveKeyBinding(const Binds::Key& name)
{
	assert(m_key_bindings.contains(name));
	m_key_bindings.erase(name);
}
#endif
#if JOYSTICK_ENABLED
bool InputHandler::GetJoystickButtonHeld(const uint& id, const uint& button) const
{
	int index = button + id * sf::Joystick::ButtonCount;
	return m_current_button_joystick_state[index] && m_joystick_button_held_timer[index] >= m_held_threshold;;
}
bool fge::InputHandler::GetJoystickButtonPressed(const uint& id, const uint& button) const
{
	int index = button + id * sf::Joystick::ButtonCount;
	return m_current_button_joystick_state[index] && !m_previous_button_joystick_state[index];
}
bool InputHandler::GetJoystickButtonReleased(const uint& id, const uint& button) const
{
	return !GetJoystickButtonPressed(id, button);
}

bool InputHandler::GetJoystickButtonHeld(const uint& id, const JoystickButton& name) const
{
	assert(m_joystick_button_bindings.contains(name));
	return GetJoystickButtonHeld(id, m_joystick_button_bindings.at(name));
}
bool InputHandler::GetJoystickButtonPressed(const uint& id, const JoystickButton& name) const
{
	assert(m_joystick_button_bindings.contains(name));
	return GetJoystickButtonPressed(id, m_joystick_button_bindings.at(name));
}

bool InputHandler::GetJoystickButtonReleased(const uint& id, const JoystickButton& name) const
{
	assert(m_joystick_button_bindings.contains(name));
	return GetJoystickButtonReleased(id, m_joystick_button_bindings.at(name));
}

float InputHandler::GetJoystickAxis(const uint& id, const uint& axis) const
{
	return m_joystick_axis[axis + id * sf::Joystick::AxisCount];
}

float InputHandler::GetJoystickAxis(const uint& id, const JoystickAxis& name) const
{
	assert(m_joystick_axis_bindings.contains(name));
	return GetJoystickAxis(id, m_joystick_axis_bindings.at(name));
}

void InputHandler::SetJoystickButtonBinding(const JoystickButton& name, const uint& button)
{
	m_joystick_button_bindings[name] = button;
}

void InputHandler::SetJoystickAxisBinding(const JoystickAxis& name, const uint& axis)
{
	m_joystick_axis_bindings[name] = axis;
}

void InputHandler::RemoveJoystickButtonBinding(const JoystickButton& name)
{
	assert(m_joystick_button_bindings.contains(name));
	m_joystick_button_bindings.erase(name);
}

void InputHandler::RemoveJoystickAxisBinding(const JoystickAxis& name)
{
	assert(m_joystick_axis_bindings.contains(name));
	m_joystick_axis_bindings.erase(name);
}
#endif

void InputHandler::SetKeyboardEnabled(bool flag) noexcept
{
	m_keyboard_enabled = flag;
}
void InputHandler::SetMouseEnabled(bool flag) noexcept
{
	m_mouse_enabled = flag;
}
void InputHandler::SetJoystickEnabled(bool flag) noexcept
{
	m_joystick_enabled = flag;
}
