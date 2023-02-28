#include <Velox/Graphics/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), m_render_system(entity_admin, id)
{
	m_render_system.Each([this](const EntityID entity, Object& object, Transform& transform, Sprite& sprite)
		{
			DrawObject(object, sprite, transform.GetTransform(), sprite.GetDepth());
		});
}

constexpr bool RenderSystem::IsRequired() const noexcept
{
	return true;
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

void RenderSystem::FixedUpdate()
{

}

void RenderSystem::PostUpdate()
{
	m_update_static_bash = false;
	m_update_static_gui_bash = false;
}

void RenderSystem::DrawObject(const Object& object, const IBatchable& batchable, const sf::Transform& transform, const float depth)
{
	if (!object.IsVisible)
		return;

	if (!object.IsGUI)
	{
		if (object.IsStatic)
		{
			if (m_update_static_bash)
				m_static_batch.Batch(batchable, transform, depth);
		}
		else
		{
			m_dynamic_batch.Batch(batchable, transform, depth);
		}
	}
	else
	{
		if (object.IsStatic)
		{
			if (m_update_static_gui_bash)
				m_static_gui_batch.Batch(batchable, transform, depth);
		}
		else
		{
			m_dynamic_gui_batch.Batch(batchable, transform, depth);
		}
	}
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