#include <Velox/Input/JoystickInput.h>

using namespace vlx;

JoystickInput::JoystickInput()
{
	ConnectAll();
}

bool JoystickInput::Held(uint32 id, ButtonType button) const
{
	const auto& joystick = m_joysticks[id];

	if (!m_enabled || !joystick)
		return false;

	return joystick->current_state[button] && joystick->held_time[button] >= m_held_threshold;
}
bool JoystickInput::Pressed(uint32 id, ButtonType button) const
{
	const auto& joystick = m_joysticks[id];

	if (!m_enabled || !joystick)
		return false;

	return joystick->current_state[button] && !joystick->previous_state[button];
}
bool JoystickInput::Released(uint32 id, ButtonType button) const
{
	const auto& joystick = m_joysticks[id];

	if (!m_enabled || !joystick)
		return false;

	return !joystick->current_state[button] && joystick->previous_state[button];
}

float JoystickInput::Axis(uint32 id, sf::Joystick::Axis axis) const
{
	const auto& joystick = m_joysticks[id];

	if (!m_enabled || !joystick)
		return false;

	return joystick->axis[axis];
}

std::vector<uint32> JoystickInput::GetConnected() const
{
	std::vector<uint32> result;

	for (auto& joystick : m_joysticks)
	{
		if (joystick != nullptr) 
			result.emplace_back(joystick->id);
	}

	return result;
}

void JoystickInput::ConnectAll()
{
	for (uint32 i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
	{
		if (sf::Joystick::isConnected(i) && !m_joysticks[i])
			Connect(i);
	}
}

void JoystickInput::DisconnectAll()
{
	for (auto& joystick : m_joysticks)
		joystick.reset();
}

void JoystickInput::Update(const Time& time, bool focus)
{
	if (!m_enabled)
		return;

	for (auto& joystick : m_joysticks)
	{
		if (!joystick)
			continue;

		for (uint32 i = 0; i < joystick->button_count; ++i)
		{
			joystick->previous_state[i]	= joystick->current_state[i];
			joystick->current_state[i]	= focus && sf::Joystick::isButtonPressed(joystick->id, i);
		}

		for (uint32 i = 0; i < joystick->button_count; ++i)
		{
			joystick->held_time[i] = joystick->current_state[i] ? joystick->held_time[i] + time.GetRealDT() : 0.0f;
		}

		for (uint32 i = 0; i < sf::Joystick::AxisCount; ++i)
		{
			sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(i);
			joystick->axis[i] = joystick->has_axis[i] ? sf::Joystick::getAxisPosition(i, axis) * focus : 0.0f;
		}
	}
}

void JoystickInput::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::JoystickConnected:
		Connect(event.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
		Disconnect(event.joystickConnect.joystickId);
		break;
	}
}

void JoystickInput::Connect(uint32 id)
{
	m_joysticks[id] = std::make_unique<Joystick>(id);
}

void JoystickInput::Disconnect(uint32 id)
{
	m_joysticks[id].reset();
}
