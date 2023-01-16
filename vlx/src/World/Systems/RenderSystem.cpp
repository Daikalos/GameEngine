#include <Velox/World/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_RENDERING)
{
	m_system.Action([this](std::span<const EntityID> entities, Object* objects, Transform* transforms, Sprite* sprites)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Object& object			= objects[i];
				Sprite& sprite			= sprites[i];
				Transform& transform	= transforms[i];

				if (object.IsStatic)
				{
					if (m_update_static_bash)
						m_static_batch.Batch(sprite, transform, sprite.GetDepth());
				}
				else
				{
					m_dynamic_batch.Batch(sprite, transform, sprite.GetDepth());
				}
			}
		});
}

constexpr LayerType vlx::RenderSystem::GetID() const noexcept
{
	return LYR_RENDERING;
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

void RenderSystem::UpdateStaticBatch()
{
	m_update_static_bash = true;
}

void RenderSystem::PreUpdate()
{
	if (m_update_static_bash)
		m_static_batch.Clear();

	m_dynamic_batch.Clear();
}
void RenderSystem::Update()
{
	PreUpdate();
	m_entity_admin->RunSystems(LYR_RENDERING);
	PostUpdate();
}
void RenderSystem::PostUpdate()
{
	m_update_static_bash = false;
}

void RenderSystem::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
 	m_static_batch.draw(target, states);
	m_dynamic_batch.draw(target, states);
}