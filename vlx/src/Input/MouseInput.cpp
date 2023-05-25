#include <Velox/Input/MouseInput.h>

using namespace vlx;

bool MouseInput::ScrollUp() const noexcept
{ 
	return m_enabled && m_scroll_delta > m_scroll_threshold;
}
bool MouseInput::ScrollDown() const noexcept
{ 
	return m_enabled && m_scroll_delta < -m_scroll_threshold;
}

bool MouseInput::Held(const sf::Mouse::Button button) const
{
	return m_enabled && m_current_state[button] && m_held_time[button] >= m_held_threshold;
}
bool MouseInput::Pressed(const sf::Mouse::Button button) const
{
	return m_enabled && m_current_state[button] && !m_previous_state[button];
}
bool MouseInput::Released(const sf::Mouse::Button button) const
{
	return m_enabled && !m_current_state[button] && m_previous_state[button];
}

void MouseInput::Update(const Time& time, const bool focus)
{
	if (!m_enabled)
		return;

	m_scroll_delta = 0.0f;

	for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		m_previous_state[i] = m_current_state[i];
		m_current_state[i] = focus && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

		m_held_time[i] = m_current_state[i] ? (m_held_time[i] + time.GetRealDT()) : 0.0f;
	}
}
void MouseInput::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::MouseWheelScrolled:
		m_scroll_delta = event.mouseWheelScroll.delta;
		break;
	}
}