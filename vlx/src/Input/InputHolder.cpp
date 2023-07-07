#include <Velox/Input/InputHolder.h>

using namespace vlx;

InputHolder::InputHolder(Window& window) 
	: m_keyboard(), m_mouse(), m_joystick(), m_cursor(window) {}

const KeyboardInput& InputHolder::Keyboard() const	{ return m_keyboard; }
KeyboardInput& InputHolder::Keyboard()				{ return m_keyboard; }

const MouseInput& InputHolder::Mouse() const		{ return m_mouse; }
MouseInput& InputHolder::Mouse()					{ return m_mouse; }

const JoystickInput& InputHolder::Joystick() const	{ return m_joystick; }
JoystickInput& InputHolder::Joystick()				{ return m_joystick; }

const MouseCursor& InputHolder::Cursor() const		{ return m_cursor; }
MouseCursor& InputHolder::Cursor()					{ return m_cursor; }

void InputHolder::Update(const Time& time, bool focus)
{
	m_keyboard.Update(time, focus);
	m_mouse.Update(time, focus);
	m_joystick.Update(time, focus);
	m_cursor.Update(time, focus);
}
void InputHolder::HandleEvent(const sf::Event& event)
{
	m_keyboard.HandleEvent(event);
	m_mouse.HandleEvent(event);
	m_joystick.HandleEvent(event);
	m_cursor.HandleEvent(event);
}
