#include <Velox/Graphics/GUI/Component.h>

using namespace vlx::gui;

constexpr bool Component::IsActive() const noexcept
{
	return m_active;
}

void Component::Activate(const EntityAdmin& entity_admin)
{
	m_active = true;
	m_activate();

	IterateChildren<Component>(
		[&entity_admin](Component& component)
		{
			component.Activate(entity_admin);
		}, 
		entity_admin, false);
}

void Component::Deactivate(const EntityAdmin& entity_admin)
{
	m_active = false;
	m_deactivate();

	IterateChildren<Component>(
		[&entity_admin](Component& component)
		{
			component.Deactivate(entity_admin);
		},
		entity_admin, false);
}