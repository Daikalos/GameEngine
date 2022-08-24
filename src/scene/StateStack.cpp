#include "StateStack.h"

using namespace fge;

StateStack::StateStack(State::Context context)
	: m_context(context) { }

void StateStack::HandleEvent(const sf::Event& event)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->HandleEvent(event))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::PreUpdate(Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PreUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::Update(Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->Update(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::FixedUpdate(Time& time)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->FixedUpdate(time))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::PostUpdate(Time& time, float interp)
{
	for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
	{
		if (!(*it)->PostUpdate(time, interp))
			break;
	}

	ApplyPendingChanges();
}

void StateStack::Draw()
{
	for (State::Ptr& state : m_stack)
		state->draw();
}

void StateStack::Push(const sts::ID& state_id)
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

State::Ptr StateStack::CreateState(const sts::ID& state_id)
{
	auto found = m_factory.find(state_id);
	assert(found != m_factory.end());

	return found->second();
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
				for (State::Ptr& state : m_stack)
					state->OnDestroy();

				m_stack.clear();
			}
			break;
		}
	}

	m_pending_list.clear();
}
