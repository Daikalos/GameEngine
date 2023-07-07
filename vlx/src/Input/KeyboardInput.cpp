#include <Velox/Input/KeyboardInput.h>

using namespace vlx;

bool KeyboardInput::Held(ButtonType key) const
{
	return m_enabled && m_current_state[key] && m_held_time[key] >= m_held_threshold;
}
bool KeyboardInput::Pressed(ButtonType key) const
{
	return m_enabled && m_current_state[key] && !m_previous_state[key];
}
bool KeyboardInput::Released(ButtonType key) const
{
	return m_enabled && !m_current_state[key] && m_previous_state[key];
}

void KeyboardInput::Update(const Time& time, bool focus)
{
	if (!m_enabled)
		return;

	for (uint32 i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		m_previous_state[i]	= m_current_state[i];
		m_current_state[i]	= focus && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}

	for (uint32 i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		m_held_time[i] = m_current_state[i] ? (m_held_time[i] + time.GetRealDT()) : 0.0f;
	}
}

void KeyboardInput::HandleEvent(const sf::Event& event)
{
	// empty
}