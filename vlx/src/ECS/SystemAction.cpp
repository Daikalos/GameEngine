#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/EntityAdmin.h>

using namespace vlx;

SystemAction::SystemAction(EntityAdmin& entity_admin, LayerType layer, bool required)
	: m_entity_admin(&entity_admin), m_layer(layer), m_required(required) {}

bool SystemAction::IsRequired() const noexcept
{
	return m_required;
}

void SystemAction::SetEnabled(bool flag)
{
	m_enabled = flag;
}

LayerType SystemAction::GetLayer() const noexcept
{
	return m_layer;
}

bool SystemAction::GetEnabled() const noexcept
{
	return m_enabled;
}

void SystemAction::Execute() const
{
	if (GetEnabled())
		m_entity_admin->RunSystems(m_layer);
}

void SystemAction::Execute(const SystemBase& system)
{
	if (GetEnabled())
		m_entity_admin->RunSystem(&system);
}

void SystemAction::Execute(std::span<const SystemBase*> systems)
{
	if (!GetEnabled())
		return;

	for (const SystemBase* system : systems)
		m_entity_admin->RunSystem(system);
}
