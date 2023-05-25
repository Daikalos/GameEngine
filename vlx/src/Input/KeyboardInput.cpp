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

	for (int i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		m_previous_state[i] = m_current_state[i];
		m_current_state[i] = focus && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

		m_held_time[i] = m_current_state[i] ? (m_held_time[i] + time.GetRealDT()) : 0.0f;
	}
}

void KeyboardInput::HandleEvent(const sf::Event& event)
{
	// empty
}