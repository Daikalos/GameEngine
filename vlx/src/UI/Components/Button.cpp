#include <Velox/UI/Components/Button.h>

using namespace vlx::ui;

void Button::Click()
{
    m_call_clicked = m_pressed;
}

void Button::Press()
{
    m_call_pressed = !m_pressed;
    m_pressed = true;
}

void Button::Release()
{
    if (m_pressed)
    {
        m_call_released = true;
        m_pressed = false;
    }
}

void Button::Enter()
{
    m_call_entered = !m_entered;
    m_entered = true;
}

void Button::Exit()
{
    if (m_entered)
    {
        m_call_exited = true;
        m_entered = false;
    }
}
