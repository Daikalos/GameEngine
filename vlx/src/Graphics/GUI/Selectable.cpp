#include <Velox/Graphics/GUI/Selectable.h>

using namespace vlx::gui;

Selectable::Selectable(const Vector2Type& size)
	: m_size(size)
{

}
Selectable::Selectable(const Vector2Type& size, bool selectable)
	: m_size(size), m_selectable(selectable)
{

}

bool Selectable::IsSelected() const noexcept
{
	return m_selected;
}
bool Selectable::IsSelectable() const noexcept
{
	return m_selectable;
}