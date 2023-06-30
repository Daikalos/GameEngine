#include <Velox/Graphics/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin, LayerType id, Time& time)
	: SystemAction(entity_admin, id, true), 

	m_sprites(entity_admin, id), 
	m_meshes(entity_admin, id),

	m_sprites_bodies(entity_admin, id),
	m_meshes_bodies(entity_admin, id)
{
	m_sprites.Each(
		[this](EntityID eid, Renderable& r, Sprite& s, GlobalTransformMatrix& gtm)
		{
			BatchEntity(r, s, gtm.matrix, s.GetDepth());
		});

	m_meshes.Each(
		[this](EntityID eid, Renderable& r, Mesh& m, GlobalTransformMatrix& gtm)
		{
			BatchEntity(r, m, gtm.matrix, m.GetDepth());
		});

	m_sprites_bodies.Each(
		[this, &time](EntityID eid, Renderable& r, Sprite& s, PhysicsBody& pb, PhysicsBodyTransform& pbt, Transform& t, TransformMatrix& tm)
		{
			BatchBody(time, r, s, pb, pbt, t, tm, s.GetDepth());
		});

	m_meshes_bodies.Each(
		[this, &time](EntityID eid, Renderable& r, Mesh& m, PhysicsBody& pb, PhysicsBodyTransform& pbt, Transform& t, TransformMatrix& tm)
		{
			BatchBody(time, r, m, pb, pbt, t, tm, m.GetDepth());
		});

	m_sprites.Exclude<PhysicsBody, PhysicsBodyTransform>();
	m_meshes.Exclude<PhysicsBody, PhysicsBodyTransform>();
}

void RenderSystem::SetBatchMode(BatchMode batch_mode)
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
	m_update_static_batch = true;
}

void RenderSystem::SetGUIBatchMode(BatchMode batch_mode)
{
	m_static_gui_batch.SetBatchMode(batch_mode);
	m_dynamic_gui_batch.SetBatchMode(batch_mode);
}

void RenderSystem::SetGUIBatchingEnabled(bool flag)
{
	m_gui_batching_enabled = flag;
}

void RenderSystem::UpdateStaticGUIBatch()
{
	m_update_static_gui_batch = true;
}

void RenderSystem::PreUpdate()
{
	if (m_update_static_batch)
		m_static_batch.Clear();
	if (m_update_static_gui_batch)
		m_static_gui_batch.Clear();

	m_dynamic_batch.Clear();
	m_dynamic_gui_batch.Clear();
}

void RenderSystem::Update()
{
	Execute(m_sprites);
	Execute(m_meshes);
}

void RenderSystem::PostUpdate()
{
	Execute(m_sprites_bodies);
	Execute(m_meshes_bodies);

	m_update_static_batch = false;
	m_update_static_gui_batch = false;
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

void RenderSystem::BatchEntity(const Renderable& renderable, const IBatchable& batchable, const Mat4f& transform, float depth)
{
	if (!renderable.IsVisible || renderable.IsCulled)
		return;

	if (!renderable.IsGUI)
	{
		if (renderable.IsStatic)
		{
			if (m_update_static_batch)
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
			if (m_update_static_gui_batch)
				m_static_gui_batch.Batch(batchable, transform, depth);
		}
		else
		{
			m_dynamic_gui_batch.Batch(batchable, transform, depth);
		}
	}
}

void RenderSystem::BatchBody(
	const Time& time, 
	const Renderable& renderable, 
	const IBatchable& batchable, 
	const PhysicsBody& pb,
	const PhysicsBodyTransform& pbt,
	const Transform& t, 
	const TransformMatrix& tm, 
	float depth)
{
	if (pb.GetType() != BodyType::Dynamic || !pb.IsAwake() || !pb.IsEnabled()) // draw normally if not moved by physics
	{
		BatchEntity(renderable, batchable, tm.matrix, depth);
	}
	else if (t.GetPosition() == pbt.GetLastPosition() && t.GetRotation() == pbt.GetLastRotation()) // draw normally if havent moved at all
	{
		BatchEntity(renderable, batchable, tm.matrix, depth);
	}
	else
	{
		Vector2f lerp_pos = Vector2f::Lerp(pbt.GetLastPosition(), t.GetPosition(), time.GetAlpha());
		sf::Angle lerp_rot = au::Lerp(pbt.GetLastRotation(), t.GetRotation(), time.GetAlpha());

		Mat4f transform;
		transform.Build(lerp_pos, t.GetOrigin(), t.GetScale(), lerp_rot);

		BatchEntity(renderable, batchable, transform, depth);
	}
}