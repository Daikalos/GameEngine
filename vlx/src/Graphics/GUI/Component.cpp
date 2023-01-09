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

constexpr bool Component::IsSelected() const noexcept
{
	return m_selected;
}
constexpr bool Component::IsSelectable() const noexcept
{
	return m_selectable;
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