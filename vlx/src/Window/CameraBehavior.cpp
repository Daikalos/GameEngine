#include <Velox/Window/CameraBehavior.h>

using namespace vlx;

CameraBehavior::Context::Context(const Window& window, const InputHolder& inputs)
	: window(&window), inputs(&inputs) {}

CameraBehavior::CameraBehavior(ID id, Camera& camera, Context context) 
	: m_id(id), m_camera(&camera), m_context(context) {}

auto CameraBehavior::GetID() const noexcept -> CameraBehavior::ID
{
	return m_id; 
}

Camera& CameraBehavior::GetCamera() const 
{ 
	return *m_camera; 
}
const CameraBehavior::Context& CameraBehavior::GetContext() const 
{ 
	return m_context; 
}