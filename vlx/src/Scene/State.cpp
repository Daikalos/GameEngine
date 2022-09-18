#include <Velox/Scene/State.h>

using namespace vlx;

State::Context::Context(Window& window, Camera& camera, ControlMap& controls, TextureHolder& texture_holder, FontHolder& font_holder)
	: window(&window), camera(&camera), controls(&controls), texture_holder(&texture_holder), font_holder(&font_holder)
{ 

}

State::State(const ID id, StateStack& state_stack, Context context)
	: m_id(id), m_state_stack(&state_stack), m_context(context) 
{

}

State::~State() = default;

const State::ID& State::GetID() const noexcept
{
	return m_id;
}

const State::Context& State::GetContext() const 
{ 
	return m_context;
}
StateStack& State::GetStack() const 
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