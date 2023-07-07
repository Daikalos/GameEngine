#include <Velox/Window/CameraBehavior.h>

using namespace vlx;

CameraBehavior::Context::Context(const Window& window, const InputHolder& inputs)
	: window(&window), inputs(&inputs) {}

CameraBehavior::CameraBehavior(ID id, Camera& camera, const Context& context) 
	: m_id(id), m_camera(&camera), m_context(context) {}

auto CameraBehavior::GetID() const noexcept -> CameraBehavior::ID
{
	return m_id; 
}

Camera& CameraBehavior::GetCamera() const 
{ 
	return *m_camera; 
}
auto CameraBehavior::GetContext() const -> const CameraBehavior::Context&
{ 
	return m_context; 
}