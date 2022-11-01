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

			bool& prev_state = m_previous_state[k];
			bool& curr_state = m_current_state[k];
			float& held_time = m_held_time[k];

			prev_state = curr_state;
			curr_state = focus && m_enabled && sf::Joystick::isButtonPressed(i, j);

			held_time = curr_state ?
				held_time + (held_time < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
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

		std::fill_n(m_previous_state + dest_btn, sf::Joystick::ButtonCount, false); // be sure to set all to default state
		std::fill_n(m_current_state + dest_btn, sf::Joystick::ButtonCount, false);
		std::fill_n(m_held_time + dest_btn, sf::Joystick::ButtonCount, 0.0f);

		std::fill_n(m_axis + dest_axis, sf::Joystick::AxisCount, 0.0f);

		if (!cu::SwapPop(m_available, event.joystickConnect.joystickId))
			throw std::runtime_error("A nonexisting joystick has been prompted to be removed");
	}
	break;
	}
}

bool JoystickInput::Held(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && m_held_time[index] >= m_held_threshold;;
}
bool JoystickInput::Pressed(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && !m_previous_state[index];
}
bool JoystickInput::Released(const uint32_t id, const uint32_t button) const
{
	const int index = button + id * sf::Joystick::ButtonCount;
	return m_current_state[index] && !m_previous_state[index];
}

float JoystickInput::Axis(const uint32_t id, const uint32_t axis) const
{
	return m_axis[axis + id * sf::Joystick::AxisCount];
}