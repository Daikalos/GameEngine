#include <Velox/Input/InputHandler.h>

using namespace vlx;

void InputHandler::SetEnabled(const bool flag) noexcept
{
	m_enabled = flag;
}

void InputHandler::SetHeldThreshold(const float value) noexcept
{
	m_held_threshold = value;
}