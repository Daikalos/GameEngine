#include <Velox/Window/Camera.h>

using namespace vlx;

Camera::PendingChange::PendingChange(const Action& action, const CameraBehavior::ID camera_id)
	: action(action), camera_id(camera_id) 
{

}

Camera::Camera(CameraBehavior::Context context) 
	: m_context(context), m_position(0, 0), m_scale(1, 1), m_size(0, 0) 
{

}

const sf::Transform& Camera::GetViewMatrix() const
{
	if (m_update_transform)
	{
		m_transform = sf::Transform()
			.translate(m_position)
			.scale(1.0f / m_scale)
			.translate(m_size / -2.0f);

		m_update_transform = false;
	}

	return m_transform;
}
sf::Vector2f Camera::ViewToWorld(const sf::Vector2f& position) const 
{ 
	return GetViewMatrix() * position; 
}
sf::Vector2f vlx::Camera::GetMouseWorldPosition(const sf::Vector2f& relative_pos) const
{
	return ViewToWorld(relative_pos);
}
sf::Vector2i vlx::Camera::GetMouseWorldPosition(const sf::Vector2i& relative_pos) const
{
	return sf::Vector2i(GetMouseWorldPosition(sf::Vector2f(relative_pos)));
}
sf::Vector2f Camera::GetMouseWorldPosition(const sf::WindowBase& window) const 
{ 
	return GetMouseWorldPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
}

const sf::Vector2f& Camera::GetPosition() const noexcept
{ 
	return m_position; 
}
const sf::Vector2f& Camera::GetScale() const noexcept
{ 
	return m_scale; 
}
const sf::Vector2f& Camera::GetSize() const noexcept
{ 
	return m_size; 
}

sf::Vector2f Camera::GetOrigin() const
{
	return GetPosition() + (GetSize() / 2.0f);
}

const CameraBehavior* Camera::GetBehavior(const CameraBehavior::ID camera_id) const
{
	return const_cast<Camera&>(static_cast<const Camera&>(*this)).GetBehavior(camera_id);
}
CameraBehavior* Camera::GetBehavior(const CameraBehavior::ID camera_id)
{
	for (const CameraBehavior::Ptr& behavior : m_stack)
		if (behavior->GetID() == camera_id)
			return behavior.get();

	return nullptr;
}

void Camera::SetPosition(const sf::Vector2f& position)
{
	setCenter(position);
	m_position = position;

	m_update_transform = true;
}
void Camera::SetScale(const sf::Vector2f& scale)
{
	setSize(m_size * (1.0f / scale));
	m_scale = scale;

	m_update_transform = true;
}
void Camera::SetSize(const sf::Vector2f& size)
{
	setSize(size * (1.0f / m_scale));
	m_size = size;

	m_update_transform = true;
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
void Camera::PostUpdate(const Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PostUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void Camera::Pop()
{
	m_pending_list.push_back(PendingChange(Action::Pop));
}
void Camera::Erase(const CameraBehavior::ID camera_id)
{
	m_pending_list.push_back(PendingChange(Action::Erase, camera_id));
}
void Camera::Clear()
{
	m_pending_list.push_back(PendingChange(Action::Clear));
}

CameraBehavior::Ptr Camera::CreateBehavior(const CameraBehavior::ID camera_id)
{
	auto found = m_factory.find(camera_id);
	assert(found != m_factory.end());
	return found->second();
}

void Camera::ApplyPendingChanges()
{
	const auto pop = [this]()
	{
		m_stack.back()->OnDestroy();
		m_stack.pop_back();

		if (!m_stack.empty())
			m_stack.back()->OnActivate();
	};

	for (const PendingChange& change : m_pending_list)
	{
		switch (change.action)
		{
		case Action::Push:
			m_stack.push_back(CreateBehavior(change.camera_id));
			m_stack.back()->OnCreate(change.data);
			break;
		case Action::Pop:
			pop();
			break;
		case Action::Erase:
			{
				auto it = std::find_if(m_stack.begin(), m_stack.end(), [&change](const CameraBehavior::Ptr& ptr)
					{ return ptr->GetID() == change.camera_id; });

				if (it == m_stack.end())
					break;

				if (it != m_stack.end() - 1) // if not last
				{
					(*it)->OnDestroy();
					m_stack.erase(it);
				}
				else // if this is the last
					pop();
			}
			break;
		case Action::Clear:
			{
				for (CameraBehavior::Ptr& behaviour : m_stack)
					behaviour->OnDestroy();

				m_stack.clear();
			}
			break;
		}
	}

	m_pending_list.clear();
}
