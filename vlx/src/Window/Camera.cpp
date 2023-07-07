#include <Velox/Window/Camera.h>

using namespace vlx;

Camera::PendingChange::PendingChange(Action action, BehaviorID behavior_id)
	: action(action), behavior_id(behavior_id) { }

Camera::Camera(const BehaviorContext& context)
	: m_context(context), m_position(0, 0), m_scale(1, 1), m_size(0, 0) { }

const Mat4f& Camera::GetViewMatrix() const
{
	if (m_update_transform)
	{
		m_transform = Mat4f()
			.Translate(m_position)
			.Scale(1.0f / m_scale)
			.Translate(m_size / -2.0f);

		m_update_transform = false;
	}

	return m_transform;
}
Vector2f Camera::ViewToWorld(const Vector2f& position) const 
{ 
	return GetViewMatrix() * position; 
}
Vector2f Camera::GetMouseWorldPosition(const Vector2f& relative_pos) const
{
	return ViewToWorld(relative_pos);
}
Vector2i Camera::GetMouseWorldPosition(const Vector2i& relative_pos) const
{
	return Vector2i(GetMouseWorldPosition(Vector2f(relative_pos)));
}
Vector2f Camera::GetMouseWorldPosition(const sf::WindowBase& window) const 
{ 
	return GetMouseWorldPosition(Vector2f(sf::Mouse::getPosition(window)));
}

const Vector2f& Camera::GetPosition() const noexcept
{ 
	return m_position; 
}
const Vector2f& Camera::GetScale() const noexcept
{ 
	return m_scale; 
}
const Vector2f& Camera::GetSize() const noexcept
{ 
	return m_size; 
}

Vector2f Camera::GetOrigin() const
{
	return GetPosition() + (GetSize() / 2.0f);
}

CameraBehavior* Camera::GetBehavior(BehaviorID behavior_id)
{
	for (const BehaviorPtr& behavior : m_stack)
		if (behavior->GetID() == behavior_id)
			return behavior.get();

	return nullptr;
}
const CameraBehavior* Camera::GetBehavior(BehaviorID behavior_id) const
{
	return const_cast<Camera&>(*this).GetBehavior(behavior_id);
}

void Camera::SetPosition(const Vector2f& position)
{
	setCenter(position);
	m_position = position;

	m_update_transform = true;
}
void Camera::SetScale(const Vector2f& scale)
{
	setSize(m_size * (1.0f / scale));
	m_scale = scale;

	m_update_transform = true;
}
void Camera::SetSize(const Vector2f& size)
{
	setSize(size * (1.0f / m_scale));
	m_size = size;

	m_update_transform = true;
}

void Camera::HandleEvent(const sf::Event& event)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&event](const BehaviorPtr& ptr)
		{
			return ptr->HandleEvent(event);
		});

	ApplyPendingChanges();
}

void Camera::Start(const Time& time)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&time](const BehaviorPtr& ptr)
		{
			return ptr->Start(time);
		});

	ApplyPendingChanges();
}

void Camera::PreUpdate(const Time& time)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&time](const BehaviorPtr& ptr)
		{
			return ptr->PreUpdate(time);
		});

	ApplyPendingChanges();
}
void Camera::Update(const Time& time)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&time](const BehaviorPtr& ptr)
		{
			return ptr->Update(time);
		});

	ApplyPendingChanges();
}
void Camera::FixedUpdate(const Time& time)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&time](const BehaviorPtr& ptr)
		{
			return ptr->FixedUpdate(time);
		});

	ApplyPendingChanges();
}
void Camera::PostUpdate(const Time& time)
{
	(void)std::all_of(m_stack.rbegin(), m_stack.rend(),
		[&time](const BehaviorPtr& ptr)
		{
			return ptr->PostUpdate(time);
		});

	ApplyPendingChanges();
}

void Camera::Pop()
{
	m_pending_list.emplace_back(PendingChange(Action::Pop));
}
void Camera::Erase(BehaviorID behavior_id)
{
	m_pending_list.emplace_back(PendingChange(Action::Erase, behavior_id));
}
void Camera::Clear()
{
	m_pending_list.emplace_back(PendingChange(Action::Clear));
}

auto Camera::CreateBehavior(BehaviorID behavior_id) -> BehaviorPtr
{
	const auto it = m_factory.find(behavior_id);
	if (it == m_factory.end())
		throw std::runtime_error("Behaviour could not be found");

	return it->second();
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
			m_stack.push_back(CreateBehavior(change.behavior_id));
			m_stack.back()->OnCreate(change.data);
			break;
		case Action::Pop:
			pop();
			break;
		case Action::Erase:
			{
				auto it = std::find_if(m_stack.begin(), m_stack.end(), 
					[&change](const BehaviorPtr& ptr)
					{ 
						return ptr->GetID() == change.behavior_id;
					});

				if (it == m_stack.end())
					break;

				if (it != m_stack.end() - 1) // if not last
				{
					(*it)->OnDestroy();
					m_stack.erase(it);
				}
				else pop(); // if this is the last
			}
			break;
		case Action::Clear:
			{
				for (BehaviorPtr& behaviour : m_stack)
					behaviour->OnDestroy();

				m_stack.clear();
			}
			break;
		default:
			throw std::runtime_error("Invalid action");
		}
	}

	m_pending_list.clear();
}
