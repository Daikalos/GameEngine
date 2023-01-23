#include <Velox/World/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemObject(entity_admin, id), m_system(entity_admin, id)
{
	m_system.Action([this](std::span<const EntityID> entities, Object* objects, Transform* transforms, Sprite* sprites)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Object& object			= objects[i];
				Transform& transform	= transforms[i];
				Sprite& sprite			= sprites[i];

				if (!object.IsVisible)
					continue;

				if (!object.IsGUI)
				{
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
				else
				{
					if (object.IsStatic)
					{
						if (m_update_static_gui_bash)
							m_static_gui_batch.Batch(sprite, transform, sprite.GetDepth());
					}
					else
					{
						m_dynamic_gui_batch.Batch(sprite, transform, sprite.GetDepth());
					}
				}
			}
		});
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

void RenderSystem::SetGUIBatchMode(const BatchMode batch_mode)
{
	m_static_gui_batch.SetBatchMode(batch_mode);
	m_dynamic_gui_batch.SetBatchMode(batch_mode);
}

void RenderSystem::SetGUIBatchingEnabled(const bool flag)
{
	m_gui_batching_enabled = flag;
}

void RenderSystem::UpdateStaticGUIBatch()
{
	m_update_static_gui_bash = true;
}

void RenderSystem::PreUpdate()
{
	if (m_update_static_bash)
		m_static_batch.Clear();
	if (m_update_static_gui_bash)
		m_static_gui_batch.Clear();

	m_dynamic_batch.Clear();
	m_dynamic_gui_batch.Clear();
}
void RenderSystem::Update()
{
	m_entity_admin->RunSystems(GetID());
}
void RenderSystem::PostUpdate()
{
	m_update_static_bash = false;
	m_update_static_gui_bash = false;
}

void RenderSystem::Draw(Window& window) const
{
	window.draw(m_static_batch);
	window.draw(m_dynamic_batch);
}

void RenderSystem::DrawGUI(Window& window) const
{
	window.draw(m_static_gui_batch);
	window.draw(m_dynamic_gui_batch);
}