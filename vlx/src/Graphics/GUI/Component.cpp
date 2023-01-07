#include <Velox/Graphics/GUI/Component.h>

using namespace vlx::gui;

Component::Component(const Vector2Type& size) 
	: m_size(size)
{

}
Component::Component(const Vector2Type& size, bool selectable)
	: m_size(size), m_selectable(selectable)
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
constexpr bool Component::IsSelectable() const noexcept
{
	return m_selectable;
}

void Component::Activate(const EntityAdmin& entity_admin)
{
	m_active = true;
	Activated();

	if (m_parent_active)
	{
		IterateChildren<Component>(
			[&entity_admin](Component& component) -> bool
			{
				component.m_parent_active = true;
				return component.m_active; // continue if true, stop if false
			},
			entity_admin, true);
	}
}

void Component::Deactivate(const EntityAdmin& entity_admin)
{
	m_active = false;
	Deactivated();

	IterateChildren<Component>(
		[&entity_admin](Component& component) -> bool
		{
			if (!component.m_parent_active)
				return false;

			component.m_parent_active = false;

			return true;
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