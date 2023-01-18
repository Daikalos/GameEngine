#include <Velox/Input/MouseCursor.h>

using namespace vlx;

MouseCursor::MouseCursor(Window& window, const sf::Texture& texture) 
	: MouseCursor(window, texture, m_sensitivity, m_locked) {}

MouseCursor::MouseCursor(Window& window, const sf::Texture& texture, bool locked) 
	: MouseCursor(window, texture, m_sensitivity, locked) {}

MouseCursor::MouseCursor(Window& window, const sf::Texture& texture, float sensitivity)
	: MouseCursor(window, texture, sensitivity, m_locked) {}

MouseCursor::MouseCursor(Window& window, const sf::Texture& texture, float sensitivity, bool locked)
	: m_window(&window), m_sensitivity(sensitivity), m_locked(locked)
{
	SetTexture(texture);
}

const sf::Vector2i& MouseCursor::GetPosition() const noexcept
{ 
	return m_position;
}
const sf::Vector2f& MouseCursor::GetDelta() const noexcept
{ 
	return m_delta; 
}
const float MouseCursor::GetSensitivity() const noexcept
{
	return m_sensitivity;
}
const bool MouseCursor::GetIsLocked() const noexcept
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
void MouseCursor::SetSensitivity(const float val) noexcept
{
	m_sensitivity = val;
}
void MouseCursor::SetVisibility(const bool flag) noexcept
{
	m_window->setMouseCursorVisible(flag);
}
void MouseCursor::SetIsLocked(const bool flag) noexcept
{
	m_locked = flag;
}

void MouseCursor::Update(const Time& time, const bool focus)
{
	if (m_enabled && focus)
	{
		m_position = sf::Mouse::getPosition(*m_window);

		if (m_locked)
			sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // TODO: acts buggy if window origin is outside screen, maybe fix in future

		m_delta = sf::Vector2f(m_position - m_window->GetOrigin());
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