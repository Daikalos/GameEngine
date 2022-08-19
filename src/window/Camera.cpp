#include "Camera.h"

using namespace fge;

Camera::Camera() : m_position(0, 0), m_scale(1, 1), m_size(0, 0)
{

}

void Camera::HandleEvent(const sf::Event& event)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->FixedUpdate(time))
			break;
	}

	switch (event.type)
	{
	case sf::Event::Resized:
		SetLetterboxView(event.size.width, event.size.height);
		break;
	}
}

void Camera::PreUpdate()
{
	m_camera_behaviour->PreUpdate();
}
void Camera::Update()
{
	m_camera_behaviour->Update();
}
void Camera::FixedUpdate()
{
	m_camera_behaviour->FixedUpdate();
}
void Camera::PostUpdate()
{
	m_camera_behaviour->PostUpdate();
}

sf::Transform Camera::GetViewMatrix() const
{
	return sf::Transform()
		.translate(m_position)
		.scale(1.0f / m_scale)
		.translate(m_size / -2.0f);
}

sf::Vector2f Camera::ViewToWorld(const sf::Vector2f& position) const
{
	return GetViewMatrix() * position;
}

sf::Vector2f Camera::GetMouseWorldPosition(const sf::RenderWindow& window) const 
{ 
	return ViewToWorld(sf::Vector2f(sf::Mouse::getPosition(window))); 
}

sf::Vector2f fge::Camera::GetPosition() const noexcept
{
	return m_position;
}
sf::Vector2f fge::Camera::GetScale() const noexcept
{
	return m_scale
}

sf::Vector2f fge::Camera::GetSize() const noexcept
{
	return m_size;
}

void Camera::SetPosition(const sf::Vector2f& position)
{
	setCenter(m_position);
	m_position = position;
}
void Camera::SetScale(const sf::Vector2f& scale)
{
	setSize(m_size * (1.0f / scale));
	m_scale = scale;
}
void Camera::SetSize(const sf::Vector2f& size)
{
	m_size = size;
	setSize(m_size * (1.0f / m_scale));
}

void Camera::SetLetterboxView(int width, int height)
{
	float window_ratio = width / (float)height;
	float view_ratio = getSize().x / (float)getSize().y;

	float size_x = 1.0f;
	float size_y = 1.0f;
	float pos_x = 0.0f;
	float pos_y = 0.0f;

	if (window_ratio >= view_ratio)
	{
		size_x = view_ratio / window_ratio;
		pos_x = (1.0f - size_x) / 2.0f;
	}
	else
	{
		size_y = window_ratio / view_ratio;
		pos_y = (1.0f - size_y) / 2.0f;
	}

	setViewport(sf::FloatRect(sf::Vector2f(pos_x, pos_y), sf::Vector2f(size_x, size_y)));	
}
