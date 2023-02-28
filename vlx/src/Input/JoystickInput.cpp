#include <Velox/Input/JoystickInput.h>

using namespace vlx;

JoystickInput::JoystickInput()
{
	for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
		if (sf::Joystick::isConnected(i))
			m_available[i] = true;
}

bool JoystickInput::Held(const SizeType id, const SizeType button) const
{
	if (!m_enabled || !m_available[id])
		return false;

	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && m_held_time[index] >= m_held_threshold;;
}
bool JoystickInput::Pressed(const SizeType id, const SizeType button) const
{
	if (!m_enabled || !m_available[id])
		return false;

	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && !m_previous_state[index];
}
bool JoystickInput::Released(const SizeType id, const SizeType button) const
{
	if (!m_enabled || !m_available[id])
		return false;

	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && !m_previous_state[index];
}

float JoystickInput::Axis(const SizeType id, const SizeType axis) const
{
	if (!m_enabled || !m_available[id])
		return 0.0f;

	return m_axis[axis + id * sf::Joystick::AxisCount];
}

void JoystickInput::Update(const Time& time, const bool focus)
{
	if (!m_enabled)
		return;

	for (int i = 0; i < sf::Joystick::Count; ++i)
	{
		if (!m_available[i])
			continue;

		for (uint32_t j = 0, size = sf::Joystick::getButtonCount(i); j < size; ++j)
		{
			const uint32_t k = j + i * sf::Joystick::ButtonCount;

			bool& prev_state = m_previous_state[k];
			bool& curr_state = m_current_state[k];
			float& held_time = m_held_time[k];

			prev_state = curr_state;
			curr_state = focus && sf::Joystick::isButtonPressed(i, j);

			held_time = curr_state ? held_time + time.GetRealDT() : 0.0f;
		}

		for (uint32_t j = 0; j < sf::Joystick::AxisCount; ++j)
		{
			const uint32_t k = j + i * sf::Joystick::AxisCount;

			sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);
			m_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) * focus : 0.0f;
		}
	}
}

void JoystickInput::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::JoystickConnected:
		m_available[event.joystickConnect.joystickId] = true;
		break;
	case sf::Event::JoystickDisconnected:
		m_available[event.joystickConnect.joystickId] = false;
		break;
	}
}