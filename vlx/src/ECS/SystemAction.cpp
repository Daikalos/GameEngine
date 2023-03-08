#include <Velox/ECS/SystemAction.h>

using namespace vlx;

SystemAction::SystemAction(EntityAdmin& entity_admin, const LayerType id)
	: m_entity_admin(&entity_admin), m_id(id) {}

constexpr LayerType SystemAction::GetLayerID() const noexcept
{
	return m_id;
}

void SystemAction::Execute() const
{
	m_entity_admin->RunSystems(GetLayerID());
}
