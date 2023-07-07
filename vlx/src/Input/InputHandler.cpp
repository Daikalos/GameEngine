#include <Velox/Input/InputHandler.h>

using namespace vlx;

void InputHandler::SetEnabled(bool flag) noexcept
{
	m_enabled = flag;
}

void InputHandler::SetHeldThreshold(float value) noexcept
{
	m_held_threshold = value;
}