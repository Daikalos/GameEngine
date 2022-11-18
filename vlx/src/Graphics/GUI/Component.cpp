#include <Velox/Graphics/GUI/Component.h>

using namespace vlx::gui;

Component::Component(const Vector2Type& size) 
	: m_size(size)
{

}
Component::Component(const Vector2Type& size, bool active)
	: m_size(size), m_active(active)
{

}
Component::Component(const Vector2Type& size, float opacity)
	: m_size(size), m_opacity(opacity)
{

}
Component::Component(const Vector2Type& size, bool active, float opacity)
	: m_size(size), m_active(active), m_opacity(opacity)
{

}

constexpr bool Component::IsActive() const noexcept
{
	return m_parent_active && m_active;
}
constexpr bool Component::IsSelected() const noexcept
{
	return m_selected;
}

void Component::Activate(const EntityAdmin& entity_admin)
{
	m_active = true;
	Activated();

	IterateChildren<Component>(
		[&entity_admin](Component& component)
		{
			component.m_parent_active = true;
		},
		entity_admin, true);
}

void Component::Deactivate(const EntityAdmin& entity_admin)
{
	m_active = false;
	Deactivated();

	IterateChildren<Component>(
		[&entity_admin](Component& component)
		{
			component.m_parent_active = false;
		},
		entity_admin, true);
}

void Component::Select()
{
	if (!IsSelected())
	{
		m_selected = true;
		Selected();
	}
}
void Component::Unselect()
{
	if (IsSelected())
	{
		m_selected = false;
		Unselected();
	}
}

void Component::OnAttach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Component>& child)
{
	
}
void Component::OnDetach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Component>& child)
{

}