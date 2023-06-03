#include <Velox/Scene/State.h>

using namespace vlx;

State::State(StateStack& state_stack, World& world, StateID id)
	: m_state_stack(&state_stack), m_world(&world), m_id(id) {}

StateID State::GetID() const noexcept
{
	return m_id;
}

const StateStack& State::GetStack() const
{
	return *m_state_stack;
}
StateStack& State::GetStack()
{
	return *m_state_stack;
}

const World& State::GetWorld() const
{
	return *m_world;
}
World& State::GetWorld()
{
	return *m_world;
}