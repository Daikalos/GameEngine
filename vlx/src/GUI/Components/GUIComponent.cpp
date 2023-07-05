#include <Velox/UI/Components/GUIComponent.h>

using namespace vlx::ui;

GUIComponent::GUIComponent(uint16 width, uint16 height)
	: m_size(width, height) { }

GUIComponent::GUIComponent(const Vector2<uint16>& size)
	: GUIComponent(size.x, size.y) { }

GUIComponent::~GUIComponent() = default;

auto GUIComponent::GetSize() const noexcept -> const Vector2<uint16>&
{
	return m_size;
}
bool GUIComponent::IsActive() const noexcept
{
	return m_active;
}
bool GUIComponent::IsSelected() const noexcept
{
	return m_selected;
}

void GUIComponent::SetSize(const Vector2<uint16>& size)
{
	m_size = size;
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

void GUIComponent::Activate()
{
	if (!IsActive())
	{
		m_active = true;
		Activated();
	}
}
void GUIComponent::Deactivate()
{
	if (IsActive())
	{
		m_active = false;
		Deactivated();
	}
}