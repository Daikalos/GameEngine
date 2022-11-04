#include <Velox/Graphics/GUI/IGUI.h>

using namespace vlx::gui;

constexpr bool IGUI::IsSelected() const noexcept
{
	return m_selected;
}

void IGUI::Select()
{
	m_selected = true;
}

void IGUI::Deselect()
{
	m_selected = false;
}

constexpr bool IGUI::IsActive() const
{
	return m_active;
}

void IGUI::Activate()
{
	m_active = true;
}

void IGUI::Deactivate()
{
	m_active = false;
}

bool IGUI::Update(float dt)
{ 
	return true; 
}
bool IGUI::Draw() 
{
	return true; 
}

sf::Vector2f IGUI::GetSize() const
{
	return sf::Vector2f();
}