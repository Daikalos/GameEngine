#include <Velox/Graphics/GUI/GUIComponent.h>

using namespace vlx::gui;

GUIComponent::GUIComponent(const Vector2Type& size) : m_size(size) { }
GUIComponent::GUIComponent(const SizeType width, const SizeType height) : m_size(width, height) { }

GUIComponent::~GUIComponent() = default;

auto GUIComponent::GetSize() const noexcept -> const Vector2Type&
{
	return m_size;
}
constexpr bool GUIComponent::IsActive() const noexcept
{
	return m_active;
}

constexpr bool GUIComponent::IsSelected() const noexcept
{
	return m_selected;
}

void GUIComponent::SetSize(const Vector2Type& size)
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