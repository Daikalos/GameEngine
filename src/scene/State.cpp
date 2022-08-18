#include "State.h"
#include "StateStack.h"

using namespace fge;

void State::RequestStackPush(States::ID state_id)
{
	m_state_stack->Push(state_id);
}

void State::RequestStackPop()
{
	m_state_stack->Pop();
}

void State::RequestStackClear()
{
	m_state_stack->Clear();
}
