#include <Velox/Graphics/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin, const LayerType id, Time& time)
	: SystemAction(entity_admin, id), 

	m_render_sprites(entity_admin, id), 
	m_render_bodies(entity_admin, id)
{
	m_render_sprites.Each(
		[this](const EntityID eid, Renderable& r, Sprite& s, GlobalTransform& gt)
		{
			BatchEntity(r, s, gt.GetTransform(), s.GetDepth());
		});

	m_render_bodies.Each(
		[this, &time](const EntityID eid, Renderable& r, Sprite& s, PhysicsBody& pb, Transform& t)
		{
			if (pb.GetType() != BodyType::Dynamic || !pb.IsAwake() || !pb.IsEnabled()) // draw normally if not moved by physics
			{
				BatchEntity(r, s, t.GetTransform(), s.GetDepth());
				return;
			}

			if (pb.position == pb.last_pos && pb.rotation == pb.last_rot) // draw normally if havent moved at all
			{
				BatchEntity(r, s, t.GetTransform(), s.GetDepth());
			}
			else
			{
				Vector2f lerp_pos = Vector2f::Lerp(pb.last_pos, pb.position, time.GetAlpha());
				sf::Angle lerp_rot = au::Lerp(pb.last_rot, pb.rotation, time.GetAlpha());

				Mat4f transform;
				transform.Build(lerp_pos, t.GetOrigin(), t.GetScale(), lerp_rot);

				BatchEntity(r, s, transform, s.GetDepth());
			}
		});

	m_render_sprites.Exclude<PhysicsBody>();
}

bool RenderSystem::IsRequired() const noexcept
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
	m_render_sprites.ForceRun();
}

void RenderSystem::FixedUpdate()
{

}

void RenderSystem::PostUpdate()
{
	m_render_bodies.ForceRun();

	m_update_static_bash = false;
	m_update_static_gui_bash = false;
}

void RenderSystem::BatchEntity(const Renderable& renderable, const IBatchable& batchable, const Mat4f& transform, const float depth)
{
	if (!renderable.IsVisible)
		return;

	if (!renderable.IsGUI)
	{
		if (renderable.IsStatic)
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
		if (renderable.IsStatic)
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