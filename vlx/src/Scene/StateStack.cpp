#include <Velox/Scene/StateStack.h>

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

auto StateStack::CreateState(StateID state_id) -> typename State::Ptr
{
	auto found = m_factory.find(state_id);
	assert(found != m_factory.end());

	auto ptr = found->second();
	ptr->OnCreated();

	return std::move(ptr);
}

void StateStack::ApplyPendingChanges()
{
	for (const PendingChange& change : m_pending_list)
	{
		switch (change.action)
		{
		case Action::Push:
			m_stack.push_back(CreateState(change.state_id));
			break;
		case Action::Pop:
		{
			m_stack.back()->OnDestroy();
			m_stack.pop_back();

			if (!m_stack.empty())
				m_stack.back()->OnActivate();
		}
		break;
		case Action::Clear:
		{
			for (typename State::Ptr& state : m_stack)
				state->OnDestroy();

			m_stack.clear();
		}
		break;
		}
	}

	m_pending_list.clear();
}