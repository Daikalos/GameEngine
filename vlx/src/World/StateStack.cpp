#include <Velox/World/StateStack.h>

using namespace vlx;

StateStack::PendingChange::PendingChange(const Action& action, StateID state_id)
	: action(action), state_id(state_id) { }

StateStack::StateStack(World& world)
	: m_world(&world) { }

bool StateStack::IsEmpty() const noexcept
{
	return m_stack.empty();
}
bool StateStack::IsPaused() const noexcept
{
	return m_paused;
}

void StateStack::SetPaused(bool flag)
{
	m_paused = flag;
}

void StateStack::HandleEvent(const sf::Event& event)
{
	if (m_paused) // not sure if should be here
		return;

	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->HandleEvent(event))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::Start(Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->Start(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::PreUpdate(Time& time)
{
	if (m_paused)
		return;

	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PreUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::Update(Time& time)
{
	if (m_paused)
		return;

	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->Update(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::FixedUpdate(Time& time)
{
	if (m_paused)
		return;

	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->FixedUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::PostUpdate(Time& time)
{
	if (m_paused)
		return;

	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PostUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::Draw()
{
	for (typename State::Ptr& state : m_stack)
		state->Draw();
}

void StateStack::Push(StateID state_id)
{
	m_pending_list.push_back(PendingChange(Action::Push, state_id));
}
void StateStack::Pop()
{
	m_pending_list.push_back(PendingChange(Action::Pop));
}
void StateStack::Clear()
{
	m_pending_list.push_back(PendingChange(Action::Clear));
}

auto StateStack::CreateState(StateID state_id) -> StatePtr
{
	const auto it = m_factory.find(state_id);
	if (it == m_factory.end())
		throw std::runtime_error("State could not be found");

	return it->second();
}

void StateStack::ApplyPendingChanges()
{
	const auto PopState = [this]()
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
			m_stack.emplace_back(CreateState(change.state_id));
			m_stack.back()->OnCreate();
			break;
		case Action::Pop:
			PopState();
			break;
		case Action::Erase:
			{
				auto it = std::find_if(m_stack.begin(), m_stack.end(),
					[&change](const StatePtr& ptr)
					{
						return ptr->GetID() == change.state_id;
					});

				if (it == m_stack.end())
					break;

				if (it != m_stack.end() - 1) // if not last
				{
					(*it)->OnDestroy();
					m_stack.erase(it);
				}
				else PopState(); // if this is the last
			}
			break;
			break;
		case Action::Clear:
		{
			for (StatePtr& state : m_stack)
				state->OnDestroy();

			m_stack.clear();
		}
		break;
		default:
			throw std::runtime_error("Invalid action");
		}
	}

	m_pending_list.clear();
}