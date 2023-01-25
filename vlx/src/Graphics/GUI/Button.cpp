#include <Velox/Graphics/GUI/Button.h>

using namespace vlx::gui;

constexpr bool Button::IsSelectable() const noexcept
{
    return true;
}

void Button::Click()
{
    if (m_pressed)
    {
        Clicked();
    }
}

void Button::Press()
{
    if (!m_pressed)
    {
        Pressed();
        m_pressed = true;
    }
}

void Button::Release()
{
    if (m_pressed)
    {
        Released();
        m_pressed = false;
    }
}

void Button::Enter()
{
    if (!m_entered)
    {
        Entered();
        m_entered = true;
    }
}

void Button::Exit()
{
    if (m_entered)
    {
        Exited();
        m_entered = false;
    }
}
