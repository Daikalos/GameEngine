#include <Velox/Graphics/GUI/GUI.h>

using namespace vlx::gui;

constexpr bool GUI::IsSelected() const noexcept
{
	return m_selected;
}

void GUI::Select()
{
	m_selected = true;
}

void GUI::Deselect()
{
	m_selected = false;
}

constexpr bool GUI::IsActive() const
{
	return m_active;
}

void GUI::Activate()
{
	m_active = true;
}

void GUI::Deactivate()
{
	m_active = false;
}

bool GUI::Update(float dt)
{ 
	return true; 
}
bool GUI::Draw() 
{
	return true; 
}

sf::Vector2f GUI::GetSize() const
{
	return sf::Vector2f();
}