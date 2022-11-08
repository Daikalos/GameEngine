#include <Velox/Graphics/GUI/Component.h>

using namespace vlx::gui;

constexpr bool Component::IsSelected() const noexcept
{
	return m_selected;
}

void Component::Select()
{
	m_selected = true;
}

void Component::Deselect()
{
	m_selected = false;
}

constexpr bool Component::IsActive() const
{
	return m_active;
}

void Component::Activate()
{
	m_active = true;
}

void Component::Deactivate()
{
	m_active = false;
}

bool Component::Update(float dt)
{ 
	return true; 
}
bool Component::Draw()
{
	return true; 
}

sf::Vector2f Component::GetSize() const
{
	return sf::Vector2f();
}