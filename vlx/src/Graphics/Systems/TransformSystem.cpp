#include <Velox/Graphics/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, LayerType id)
	: SystemAction(entity_admin, id, true), 

	m_sync(entity_admin, id),
	m_dirty(entity_admin, id),
	m_dirty_descendants(entity_admin, id),
	m_update_global(entity_admin, id),
	m_update_pos(entity_admin, id),
	m_update_rot(entity_admin, id),
	m_update_scl(entity_admin, id)
{
	m_sync.Each(
		[](EntityID, Transform& t, TransformMatrix& tm)
		{
			// TODO: consider implementing TransformChanging that tracks changes to the component instead of doing it brute-force as it is now

			if (t.m_update)
			{
				tm.matrix.Build(t.GetPosition(), t.GetOrigin(), t.GetScale(), t.GetRotation());
				t.m_update = false;
			}
		});

	m_dirty.Each(
		[](EntityID, Transform& t, GlobalTransformDirty& gtd)
		{
			if (t.m_dirty) // if local is dirty, so is global transform
			{
				gtd.m_dirty = true;
				t.m_dirty = false;
			}
		});

	m_dirty_descendants.Each(
		[this](EntityID, GlobalTransformDirty& gtd, Relation& r)
		{
			if (gtd.m_dirty) // all of the children needs their transform to be updated
				DirtyDescendants(gtd, r.GetChildren());
		});

	m_update_global.Each(
		[this](EntityID, TransformMatrix& tm, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm, Relation& r)
		{
			if (gtd.m_dirty)
				UpdateTransforms(tm, gtd, gtm, r.GetParent());
		});

	m_update_pos.Each(
		[](EntityID, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm, GlobalTransformTranslation& gtt)
		{
			if (gtd.m_update_position)
			{
				gtt.m_position = gtm.matrix.GetTranslation();
				gtd.m_update_position = false;
			}
		});

	m_update_rot.Each(
		[](EntityID, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm, GlobalTransformRotation& gtr)
		{
			if (gtd.m_update_rotation)
			{
				gtr.m_rotation = gtm.matrix.GetRotation();
				gtd.m_update_rotation = false;
			}
		});

	m_update_scl.Each(
		[](EntityID, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm, GlobalTransformScale& gts)
		{
			if (gtd.m_update_scale)
			{
				gts.m_scale = gtm.matrix.GetScale();
				gtd.m_update_scale = false;
			}
		});

	// TODO: sync inverse matrix

	m_dirty.SetPriority(900.0f);
	m_dirty_descendants.SetPriority(800.0f);
	m_update_global.SetPriority(0.0f);
}

void TransformSystem::SetGlobalPosition(EntityID entity, const Vector2f& position) 
{
	auto transform	= m_entity_admin->TryGetComponent<Transform>(entity);
	auto relation	= m_entity_admin->TryGetComponent<Relation>(entity);

	if (!transform.has_value())
		return;

	if (relation.has_value())
	{
		SetGlobalPosition(*transform.value(), *relation.value(), position);
	}
	else // no relation component, set position directly
	{
		transform.value()->SetPosition(position);
	}
}
void TransformSystem::SetGlobalScale(EntityID entity, const Vector2f& scale)
{

}
void TransformSystem::SetGlobalRotation(EntityID entity, const sf::Angle angle)
{

}

void TransformSystem::SetGlobalPosition(Transform& transform, Relation& relation, const Vector2f& position)
{
	if (relation.HasParent())
	{
		GlobalTransformMatrix& pgtm = m_entity_admin->GetComponent<GlobalTransformMatrix>(relation.GetParent().GetEntityID());
		transform.SetPosition(pgtm.matrix.GetInverse() * position); // global_transform position to parent space to appear global
	}
	else transform.SetPosition(position);
}
void TransformSystem::SetGlobalScale(Transform& transform, Relation& relation, const Vector2f& scale)
{
	// TODO: implement
}
void TransformSystem::SetGlobalRotation(Transform& transform, Relation& relation, sf::Angle angle)
{
	// TODO: implement
}

void TransformSystem::Start()
{

}

void TransformSystem::PreUpdate()
{

}

void TransformSystem::Update()
{
	Execute();
}

void TransformSystem::FixedUpdate()
{

}

void TransformSystem::PostUpdate()
{
	// TODO: figure out how to keep cache at reasonable size
}

void TransformSystem::DirtyDescendants(GlobalTransformDirty& gtd, const Relation::Children& children) const
{
	const auto RecursiveClean = [this](GlobalTransformDirty& gtd, const Relation::Children& children)
	{
		auto RecursiveImpl = [this](GlobalTransformDirty& gtd, const Relation::Children& children, auto& recursive_ref) mutable -> void
		{
			gtd.m_dirty = true; // all of the children needs their transform to be updated
			for (const auto& child : children)
			{
				auto opt = CheckCache(child.GetEntityID());

				if (!opt.has_value())
					continue;

				GlobalTransformDirty* child_dirty = opt.value()->Get<GlobalTransformDirty>(); // why intellisense, oh why

				if (child_dirty != nullptr && !child_dirty->m_dirty)
					recursive_ref(*child_dirty, child->GetChildren(), recursive_ref);
			}
		};

		return RecursiveImpl(gtd, children, RecursiveImpl);
	};

	RecursiveClean(gtd, children);
}
void TransformSystem::UpdateTransforms(TransformMatrix& tm, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm, const Relation::Parent& parent) const
{
	if (!parent.IsValid())
	{
		UpdateToLocal(tm, gtd, gtm);
		return;
	}

	auto opt = CheckCache(parent.GetEntityID());
	if (!opt.has_value())
	{
		UpdateToLocal(tm, gtd, gtm);
		return;
	}

	CacheSet& parent_set = *opt.value();

	TransformMatrix* ptm		= parent_set.Get<TransformMatrix>();
	GlobalTransformDirty* pgtd	= parent_set.Get<GlobalTransformDirty>();
	GlobalTransformMatrix* pgtm = parent_set.Get<GlobalTransformMatrix>();

	if (pgtm == nullptr || pgtd == nullptr || ptm == nullptr)
	{
		UpdateToLocal(tm, gtd, gtm);
		return;
	}

	if (pgtd->m_dirty) // only update if not up-to-date
		UpdateTransforms(*ptm, *pgtd, *pgtm, parent->GetParent());

	const Mat4f matrix = pgtm->matrix * tm.matrix;
	if (matrix != gtm.matrix)
	{
		gtm.matrix = matrix;
		gtd.m_dirty = false;

		gtd.m_update_position = true;
		gtd.m_update_rotation = true;
		gtd.m_update_scale = true;
	}
}

void TransformSystem::UpdateToLocal(TransformMatrix& tm, GlobalTransformDirty& gtd, GlobalTransformMatrix& gtm) const
{
	gtm.matrix = tm.matrix;
	gtd.m_dirty = false;

	gtd.m_update_position = true;
	gtd.m_update_rotation = true;
	gtd.m_update_scale = true;

	//global_transform.m_position	= transform.m_position - transform.m_origin;
	//global_transform.m_scale		= transform.m_scale;
	//global_transform.m_rotation	= transform.m_rotation;
}

auto TransformSystem::CheckCache(EntityID entity_id) const -> std::optional<CacheSet*>
{
	const auto it = m_cache.find(entity_id);
	if (it == m_cache.end())
	{
		CacheSet set = m_entity_admin->GetComponentsRef(entity_id, CacheTuple{}); // stupid intellisense error
		const auto [eit, success] = m_cache.emplace(entity_id, set);

		return &eit->second;
	}

	if (!it->second.IsAllValid())
	{
		m_cache.erase(it);
		return std::nullopt;
	}

	return &it->second;
}
