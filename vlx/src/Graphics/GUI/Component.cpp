#include <Velox/Graphics/GUI/Component.h>

using namespace vlx::gui;

const bool& Component::IsActive() const noexcept
{
	return m_active;
}

void Component::Activate()
{
	m_active = true;
}

void Component::Deactivate()
{
	m_active = false;
}