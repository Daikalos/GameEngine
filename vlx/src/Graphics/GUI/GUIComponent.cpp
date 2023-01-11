#include <Velox/Graphics/GUI/GUIComponent.h>

using namespace vlx::gui;

GUIComponent::GUIComponent(const Vector2Type& size)
	: m_size(size)
{

}
GUIComponent::GUIComponent(const Vector2Type& size, bool selectable)
	: m_size(size), m_selectable(selectable)
{

}

constexpr bool GUIComponent::IsSelected() const noexcept
{
	return m_selected;
}
constexpr bool GUIComponent::IsSelectable() const noexcept
{
	return m_selectable;
}

void GUIComponent::Select()
{
	if (!IsSelected())
	{
		m_selected = true;
		Selected();
	}
}
void GUIComponent::Deselect()
{
	if (IsSelected())
	{
		m_selected = false;
		Deselected();
	}
}