#include <Velox/Input/KeyboardInput.h>

using namespace vlx;

bool KeyboardInput::Held(const sf::Keyboard::Key key) const
{
	return m_enabled && m_current_state[key] && m_held_time[key] >= m_held_threshold;
}
bool KeyboardInput::Pressed(const sf::Keyboard::Key key) const
{
	return m_enabled && m_current_state[key] && !m_previous_state[key];
}
bool KeyboardInput::Released(const sf::Keyboard::Key key) const
{
	return m_enabled && !m_current_state[key] && m_previous_state[key];
}

void KeyboardInput::Update(const Time& time, const bool focus)
{
	if (!m_enabled)
		return;

	for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		bool& prev_state = m_previous_state[i];
		bool& curr_state = m_current_state[i];
		float& held_time = m_held_time[i];

		prev_state = curr_state;
		curr_state = focus && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

		held_time = curr_state ? (held_time + time.GetRealDT()) : 0.0f;
	}
}

void KeyboardInput::HandleEvent(const sf::Event& event)
{
	// empty
}