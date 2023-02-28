#include <Velox/ECS/SystemAction.h>

using namespace vlx;

SystemAction::SystemAction(EntityAdmin& entity_admin, const LayerType id)
	: m_entity_admin(&entity_admin), m_id(id) {}

constexpr LayerType SystemAction::GetID() const noexcept
{
	return m_id;
}
