#include <Velox/Input/MouseCursor.h>

using namespace vlx;

MouseCursor::MouseCursor(Window& window) 
	: MouseCursor(window, m_sensitivity, m_locked) {}

MouseCursor::MouseCursor(Window& window, bool locked) 
	: MouseCursor(window, m_sensitivity, locked) {}

MouseCursor::MouseCursor(Window& window, float sensitivity)
	: MouseCursor(window, sensitivity, m_locked) {}

MouseCursor::MouseCursor(Window& window, float sensitivity, bool locked)
	: m_window(&window), m_sensitivity(sensitivity), m_locked(locked)
{
}

const Vector2i& MouseCursor::GetPosition() const noexcept
{ 
	return m_position;
}
const Vector2f& MouseCursor::GetDelta() const noexcept
{ 
	return m_delta; 
}
float MouseCursor::GetSensitivity() const noexcept
{
	return m_sensitivity;
}
bool MouseCursor::GetIsLocked() const noexcept
{
	return m_locked;
}

void MouseCursor::SetTexture(const sf::Texture& texture)
{
	if (!m_in_focus)
	{
		m_texture = &texture;
		return;
	}

	if (m_cursor.loadFromPixels(texture.copyToImage().getPixelsPtr(), texture.getSize(), { 0, 0 }))
	{
		m_window->setMouseCursor(m_cursor);
		m_texture = &texture;
	}
}
void MouseCursor::SetSensitivity(float val) noexcept
{
	m_sensitivity = val;
}
void MouseCursor::SetVisibility(bool flag) noexcept
{
	m_window->setMouseCursorVisible(flag);
}
void MouseCursor::SetIsLocked(bool flag) noexcept
{
	m_locked = flag;
}

void MouseCursor::Update(const Time& time, bool focus)
{
	if (m_enabled && focus)
	{
		m_position = sf::Mouse::getPosition(*m_window);

		if (m_locked)
			sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // TODO: acts buggy if window origin is outside screen, maybe fix in future

		m_delta = Vector2f(m_position - m_window->GetOrigin());
		m_delta = m_delta * m_window->GetRatioCmp() * m_sensitivity;
	}
}

void MouseCursor::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::GainedFocus:
		{
			m_in_focus = true;

			if (m_locked)
				sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // set cursor to middle of screen to prevent inaccurate delta values

			SetTexture(*m_texture);
		}
		break;
	case sf::Event::LostFocus:
		{
			if (m_cursor.loadFromSystem(sf::Cursor::Arrow))
				m_window->setMouseCursor(m_cursor);

			m_in_focus = false;
		}
		break;
	}
}