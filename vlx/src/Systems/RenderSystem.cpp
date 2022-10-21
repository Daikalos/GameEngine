#include <Velox/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_static_system(entity_admin, LYR_RENDERING), m_dynamic_system(entity_admin, LYR_RENDERING)
{

}

void RenderSystem::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{

}

void RenderSystem::SetBatchMode(const BatchMode batch_mode)
{
	m_static_batch.SetBatchMode(batch_mode);
	m_dynamic_batch.SetBatchMode(batch_mode);
}

void RenderSystem::SetBatchingEnabled(const bool flag)
{
	m_batching_enabled = flag;
}