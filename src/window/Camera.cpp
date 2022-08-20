#include "Camera.h"

using namespace fge;

Camera::Camera(CameraBehaviour::Context context) 
	: m_context(context), m_position(0, 0), m_scale(1, 1), m_size(0, 0)
{

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
	return m_scale;
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

void Camera::HandleEvent(const sf::Event& event)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->HandleEvent(event))
			break;
	}

	ApplyPendingChanges();
}

void Camera::PreUpdate(const Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PreUpdate(time))
			break;
	}

	ApplyPendingChanges();
}
void Camera::Update(const Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->Update(time))
			break;
	}

	ApplyPendingChanges();
}
void Camera::FixedUpdate(const Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->FixedUpdate(time))
			break;
	}

	ApplyPendingChanges();
}
void Camera::PostUpdate(const Time& time, float interp)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PostUpdate(time, interp))
			break;
	}

	ApplyPendingChanges();
}

void Camera::Erase(const Cameras::ID& camera_id)
{
	m_pending_list.push_back(PendingChange(Action::Erase, camera_id));
}
void Camera::Pop()
{
	m_pending_list.push_back(PendingChange(Action::Pop));
}
void Camera::Clear()
{
	m_pending_list.push_back(PendingChange(Action::Clear));
}

CameraBehaviour* Camera::GetBehaviour(const Cameras::ID& camera_id)
{
	for (const CameraBehaviour::Ptr& behaviour : m_stack)
		if (behaviour->GetId() == camera_id)
			return behaviour.get();

	return nullptr;
}

CameraBehaviour::Ptr Camera::CreateBehaviour(const Cameras::ID& camera_id)
{
	auto found = m_factory.find(camera_id);
	assert(found != m_factory.end());

	return found->second();
}

void Camera::ApplyPendingChanges()
{
	for (const PendingChange& change : m_pending_list)
	{
		switch (change.action)
		{
		case Action::Push:
			m_stack.push_back(CreateBehaviour(change.camera_id));
			m_stack.back()->OnCreate();
			break;
		case Action::Pop:
			{
				m_stack.back()->OnDestroy();
				m_stack.pop_back();

				if (!m_stack.empty())
					m_stack.back()->OnActivate();
			}
			break;
		case Action::Erase:
			{
				
			}
			break;
		case Action::Clear:
			{
				for (CameraBehaviour::Ptr& state : m_stack)
					state->OnDestroy();

				m_stack.clear();
			}
			break;
		}
	}

	m_pending_list.clear();
}
