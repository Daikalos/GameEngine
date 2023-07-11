#include <Velox/Graphics/Systems/RenderSystem.h>

using namespace vlx;

RenderSystem::RenderSystem(EntityAdmin& entity_admin, LayerType id, const Time& time)
	: SystemAction(entity_admin, id, true), m_time(&time),

	m_sprites(entity_admin, id), 
	m_meshes(entity_admin, id),

	m_sprites_bodies(entity_admin, id),
	m_meshes_bodies(entity_admin, id)
{
	m_sprites.Each(
		[this](EntityID eid, Renderable& r, Sprite& s, GlobalTransformMatrix& gtm)
		{
			BatchEntity<Sprite>(r, s, gtm.matrix, s.GetDepth());
		});

	m_meshes.Each(
		[this](EntityID eid, Renderable& r, Mesh& m, GlobalTransformMatrix& gtm)
		{
			BatchEntity<Mesh>(r, m, gtm.matrix, m.GetDepth());
		});

	m_sprites_bodies.Each(
		[this](EntityID eid, Renderable& r, Sprite& s, PhysicsBody& pb, PhysicsBodyTransform& pbt, Transform& t, TransformMatrix& tm)
		{
			BatchBody<Sprite>(r, s, pb, pbt, t, tm, s.GetDepth());
		});

	m_meshes_bodies.Each(
		[this](EntityID eid, Renderable& r, Mesh& m, PhysicsBody& pb, PhysicsBodyTransform& pbt, Transform& t, TransformMatrix& tm)
		{
			BatchBody<Mesh>(r, m, pb, pbt, t, tm, m.GetDepth());
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

template<IsBatchable T>
void RenderSystem::BatchEntity(const Renderable& renderable, const T& batchable, const Mat4f& transform, float depth)
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

template<IsBatchable T>
void RenderSystem::BatchBody(
	const Renderable& renderable,
	const T& batchable,
	const PhysicsBody& pb,
	const PhysicsBodyTransform& pbt,
	const Transform& t,
	const TransformMatrix& tm,
	float depth)
{
	if (pb.GetType() != BodyType::Dynamic || !pb.IsAwake() || !pb.IsEnabled()) // draw normally if not moved by physics
	{
		BatchEntity<T>(renderable, batchable, tm.matrix, depth);
	}
	else if (t.GetPosition() == pbt.GetLastPosition() && t.GetRotation() == pbt.GetLastRotation()) // draw normally if haven't moved at all
	{
		BatchEntity<T>(renderable, batchable, tm.matrix, depth);
	}
	else
	{
		Vector2f lerp_pos = Vector2f::Lerp(pbt.GetLastPosition(), t.GetPosition(), m_time->GetAlpha());
		sf::Angle lerp_rot = au::Lerp(pbt.GetLastRotation(), t.GetRotation(), m_time->GetAlpha());

		Mat4f transform;
		transform.Build(lerp_pos, t.GetOrigin(), t.GetScale(), lerp_rot);

		BatchEntity<T>(renderable, batchable, transform, depth);
	}
}

// explicit template instantiations

template void vlx::RenderSystem::BatchEntity<Sprite>(	const Renderable&, const Sprite&, const Mat4f&, float);
template void vlx::RenderSystem::BatchEntity<Mesh>(		const Renderable&, const Mesh&, const Mat4f&, float);

template void vlx::RenderSystem::BatchBody<Sprite>(		const Renderable&, const Sprite&, const PhysicsBody&, const PhysicsBodyTransform&, const Transform&, const TransformMatrix&, float);
template void vlx::RenderSystem::BatchBody<Mesh>(		const Renderable&, const Mesh&, const PhysicsBody&, const PhysicsBodyTransform&, const Transform&, const TransformMatrix&, float);