#include <Velox/Input/MouseInput.h>

using namespace vlx;

void MouseInput::Update(const Time& time, const bool focus)
{
	m_scroll_delta = 0.0f;

	for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		m_previous_button_state[i] = m_current_button_state[i];
		m_current_button_state[i] = focus && m_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

		m_held_timer[i] = m_current_button_state[i] ?
			m_held_timer[i] + (m_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
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

constexpr bool MouseInput::ScrollUp() const noexcept
{ 
	return m_scroll_delta > m_scroll_threshold; 
}
constexpr bool MouseInput::ScrollDown() const noexcept
{ 
	return m_scroll_delta < -m_scroll_threshold; 
}

bool MouseInput::Held(const sf::Mouse::Button button) const
{
	return m_current_button_state[button] && m_held_timer[button] >= m_held_threshold;
}
bool MouseInput::Pressed(const sf::Mouse::Button button) const
{
	return m_current_button_state[button] && !m_previous_button_state[button];
}
bool MouseInput::Released(const sf::Mouse::Button button) const
{
	return !Pressed(button);
}