#include <Velox/UI/Components/Button.h>

using namespace vlx::ui;

void Button::Click()
{
    if (m_pressed)
        m_flags |= E_Clicked;
}

void Button::Press()
{
    if (!m_pressed)
        m_flags |= E_Pressed;

    m_pressed = true;
}

void Button::Release()
{
    if (m_pressed)
    {
        m_flags |= E_Released;
        m_pressed = false;
    }
}

void Button::Enter()
{
    if (!m_entered)
        m_flags |= E_Entered;

    m_entered = true;
}

void Button::Exit()
{
    if (m_entered)
    {
        m_flags |= E_Exited;
        m_entered = false;
    }
}
