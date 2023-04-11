#include <Velox/Scene/State.h>

using namespace vlx;

State::State(StateID state_id, StateStack& state_stack, World& world)
	: m_id(state_id), m_state_stack(&state_stack), m_world(&world) {}

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