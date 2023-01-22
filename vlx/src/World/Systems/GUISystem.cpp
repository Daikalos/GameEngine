#include <Velox/World/Systems/GUISystem.h>

using namespace vlx::gui;

GUISystem::GUISystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemObject(entity_admin, id), 
	m_container_system(entity_admin, id),
	m_button_system(entity_admin, id),
	m_label_system(entity_admin, id)
{

}

void GUISystem::Update()
{

}