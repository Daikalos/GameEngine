#include <Velox/Window/CameraBehavior.h>

using namespace vlx;

CameraBehavior::Context::Context(const Window& window, const ControlMap& controls)
	: window(&window), controls(&controls) {}

CameraBehavior::CameraBehavior(const ID id, Camera& camera, Context context) 
	: m_id(id), m_camera(&camera), m_context(context) {}

const CameraBehavior::ID& CameraBehavior::GetID() const noexcept
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