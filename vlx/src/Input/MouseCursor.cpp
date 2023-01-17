#include <Velox/Input/MouseCursor.h>

using namespace vlx;

MouseCursor::MouseCursor(Window& window, const sf::Texture& default_texture) : m_window(&window)
{
	m_cursor_pos = sf::Vector2f(m_window->GetOrigin());
	sf::Mouse::setPosition(m_window->GetOrigin(), *m_window);

	SetTexture(default_texture);
}

const sf::Vector2i& MouseCursor::GetMousePosition() const noexcept
{ 
	return m_mouse_pos;
}
const sf::Vector2f& MouseCursor::GetDelta() const noexcept
{ 
	return m_mouse_delta; 
}
const sf::Vector2f& MouseCursor::GetPosition() const noexcept
{ 
	return m_cursor_pos; 
}

void MouseCursor::SetTexture(const sf::Texture& texture)
{
	m_cursor.setTexture(texture);
}
void MouseCursor::SetState(const cst::ID cursor_state)
{
	m_cursor_state = cursor_state;
}
void MouseCursor::SetSensitivity(const float val) noexcept
{
	m_mouse_sensitivity = val;
}
void MouseCursor::SetVisibility(const bool flag) noexcept
{
	m_cursor_visible = flag;
}
void MouseCursor::SetIsLocked(const bool flag) noexcept
{
	m_locked = flag;
}

void MouseCursor::Update(const Time& time, const bool focus)
{
	if (m_enabled && focus)
	{
		m_mouse_pos = sf::Mouse::getPosition(*m_window);
		m_mouse_delta = sf::Vector2f(m_mouse_pos - m_window->GetOrigin());
		sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // TODO: acts buggy if window origin is outside screen, maybe fix in future

		m_cursor_pos += m_mouse_delta * m_window->GetRatioCmp() * m_mouse_sensitivity;

		if (m_locked)
		{
			if (m_cursor_pos.x < 0.0f)
				m_cursor_pos.x = 0.0f;
			if (m_cursor_pos.y < 0.0f)
				m_cursor_pos.y = 0.0f;
			if (m_cursor_pos.x > m_window->getSize().x)
				m_cursor_pos.x = (float)m_window->getSize().x;
			if (m_cursor_pos.y > m_window->getSize().y)
				m_cursor_pos.y = (float)m_window->getSize().y;
		}

		m_cursor.setPosition(m_cursor_pos);
	}
}

void MouseCursor::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::GainedFocus:
		m_cursor_pos = sf::Vector2f(sf::Mouse::getPosition(*m_window));
		m_cursor.setPosition(m_cursor_pos);

		sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // set cursor to middle of screen to prevent inaccurate delta values
		break;
	case sf::Event::LostFocus:
		sf::Mouse::setPosition(sf::Vector2i(m_cursor_pos), *m_window);
		break;
	}
}

void MouseCursor::Draw()
{
	if (m_cursor_visible)
		m_window->draw(m_cursor);
}