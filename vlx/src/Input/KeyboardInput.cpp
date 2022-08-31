#include <Velox/Input/KeyboardInput.h>

using namespace vlx;

void KeyboardInput::Update(const Time& time, const bool focus)
{
	for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		m_previous_state[i] = m_current_state[i];
		m_current_state[i] = focus && m_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));

		m_held_timer[i] = m_current_state[i] ?
			m_held_timer[i] + (m_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
	}
}

void KeyboardInput::HandleEvent(const sf::Event& event)
{

}

bool KeyboardInput::Held(const sf::Keyboard::Key key) const
{
	return m_current_state[key] && m_held_timer[key] >= m_held_threshold;
}
bool KeyboardInput::Pressed(const sf::Keyboard::Key key) const
{
	return m_current_state[key] && !m_previous_state[key];
}
bool KeyboardInput::Released(const sf::Keyboard::Key key) const
{
	return !m_current_state[key] && m_previous_state[key];
}