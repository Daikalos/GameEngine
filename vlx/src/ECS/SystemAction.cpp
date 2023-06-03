#include <Velox/ECS/SystemAction.h>

using namespace vlx;

SystemAction::SystemAction(EntityAdmin& entity_admin, LayerType layer)
	: m_entity_admin(&entity_admin), m_layer(layer) {}

LayerType SystemAction::GetLayer() const noexcept
{
	return m_layer;
}

void SystemAction::Execute() const
{
	m_entity_admin->RunSystems(m_layer);
}
