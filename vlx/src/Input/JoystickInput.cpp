#include <Velox/Input/JoystickInput.h>

using namespace vlx;

JoystickInput::JoystickInput()
{
	m_available.reserve(sf::Joystick::Count);

	for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
		if (sf::Joystick::isConnected(i))
			m_available.push_back(i);
}

void JoystickInput::Update(const Time& time, const bool focus)
{
	for (const uint32_t& i : m_available)
	{
		for (uint32_t j = 0, size = sf::Joystick::getButtonCount(i); j < size; ++j)
		{
			const uint32_t k = j + i * sf::Joystick::ButtonCount;

			m_previous_button_state[k] = m_current_button_state[k];
			m_current_button_state[k] = focus && m_enabled && sf::Joystick::isButtonPressed(i, j);

			m_held_timer[k] = m_current_button_state[k] ?
				m_held_timer[k] + (m_held_timer[k] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
		}

		for (uint32_t j = 0; j < sf::Joystick::AxisCount; ++j)
		{
			const uint32_t k = j + i * sf::Joystick::AxisCount;

			sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);
			m_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) * focus * m_enabled : 0.0f;
		}
	}
}

void JoystickInput::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::JoystickConnected:
		m_available.push_back(event.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
	{
		const int dest_btn = event.joystickConnect.joystickId * sf::Joystick::ButtonCount;
		const int dest_axis = event.joystickConnect.joystickId * sf::Joystick::AxisCount;

		memset(m_previous_button_state + dest_btn, false, sizeof(bool) * sf::Joystick::ButtonCount); // be sure to set all to default state
		memset(m_current_button_state + dest_btn, false, sizeof(bool) * sf::Joystick::ButtonCount);
		memset(m_held_timer + dest_btn, 0.0f, sizeof(float) * sf::Joystick::ButtonCount);

		memset(m_axis + dest_axis, 0.0f, sizeof(float) * sf::Joystick::AxisCount);

		if (cu::Erase(m_available, event.joystickConnect.joystickId) == m_available.end())
			std::puts("A nonexisting joystick has been prompted to be removed");
	}
	break;
	}
}

bool JoystickInput::Held(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_button_state[index] && m_held_timer[index] >= m_held_threshold;;
}
bool JoystickInput::Pressed(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_button_state[index] && !m_previous_button_state[index];
}
bool JoystickInput::Released(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_button_state[index] && !m_previous_button_state[index];
}

float JoystickInput::Axis(const uint32_t id, const uint32_t axis) const
{
	return m_axis[axis + id * sf::Joystick::AxisCount];
}