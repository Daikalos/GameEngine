#include <Velox/Graphics/GUI/GUIComponent.h>

using namespace vlx::gui;

inline GUIComponent::~GUIComponent() = default;

GUIComponent::GUIComponent(const Vector2Type& size) : m_size(size) { }
GUIComponent::GUIComponent(const SizeType width, const SizeType height) : m_size(width, height) { }

constexpr bool GUIComponent::IsSelected() const noexcept
{
	return m_selected;
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