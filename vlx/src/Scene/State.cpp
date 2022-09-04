#include <Velox/Scene/State.h>

using namespace vlx;

State::State(state::ID id, StateStack& state_stack, Context context)
	: m_id(id), m_state_stack(&state_stack), m_context(context) 
{

}

State::~State() 
{ 

}

[[nodiscard]] state::ID State::GetId() const noexcept
{
	return m_id;
}

[[nodiscard]] const State::Context& State::GetContext() const 
{ 
	return m_context;
}
[[nodiscard]] StateStack& State::GetStack() const 
{ 
	return *m_state_stack; 
}

void State::OnActivate() 
{

}
void State::OnDestroy() 
{

}

bool State::PreUpdate(Time& time) 
{ 
	return true; 
}
bool State::FixedUpdate(Time& time) 
{ 
	return true; 
}
bool State::PostUpdate(Time& time)
{ 
	return true; 
}