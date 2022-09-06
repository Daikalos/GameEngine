#include <Velox/Window/CameraBehaviour.h>

using namespace vlx;

CameraBehaviour::Context::Context(const Window& window, const ControlMap& controls)
	: window(&window), controls(&controls) {}

CameraBehaviour::CameraBehaviour(const camera::ID id, Camera& camera, Context context) :
			m_id(id), m_camera(&camera), m_context(context) {}

CameraBehaviour::~CameraBehaviour() 
{

}

camera::ID CameraBehaviour::GetId() const noexcept 
{
	return m_id; 
}

Camera& CameraBehaviour::GetCamera() const 
{ 
	return *m_camera; 
}
const CameraBehaviour::Context& CameraBehaviour::GetContext() const 
{ 
	return m_context; 
}

void CameraBehaviour::OnCreate(const std::vector<std::byte>& data)
{

}
void CameraBehaviour::OnActivate() 
{

}
void CameraBehaviour::OnDestroy()
{

}

bool CameraBehaviour::PreUpdate(const Time& time) 
{ 
	return true; 
}
bool CameraBehaviour::FixedUpdate(const Time& time) 
{ 
	return true; 
}
bool CameraBehaviour::PostUpdate(const Time& time) 
{ 
	return true; 
}