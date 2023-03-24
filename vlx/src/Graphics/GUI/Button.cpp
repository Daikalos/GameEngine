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
        OnClick();
    }
}

void Button::Press()
{
    if (!m_pressed)
    {
        OnPress();
        m_pressed = true;
    }
}

void Button::Release()
{
    if (m_pressed)
    {
        OnRelease();
        m_pressed = false;
    }
}

void Button::Enter()
{
    if (!m_entered)
    {
        OnEnter();
        m_entered = true;
    }
}

void Button::Exit()
{
    if (m_entered)
    {
        OnExit();
        m_entered = false;
    }
}
