#include <Velox/Graphics/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), 
	m_dirty(entity_admin, id),
	m_dirty_descendants(entity_admin, id),
	m_update_global(entity_admin, id)
{
	m_dirty.Each(
		[this](EntityID, Transform& t, GlobalTransform& gt)
		{
			if (t.m_dirty) // if local is dirty, so is global transform
			{
				gt.m_dirty = true;
				t.m_dirty = false;
			}
		});

	m_dirty_descendants.Each(
		[this](EntityID, Transform& t, GlobalTransform& gt, Relation& r)
		{
			if (gt.m_dirty)
				DirtyDescendants(gt, r.GetChildren());
		});

	m_update_global.Each(
		[this](EntityID, Transform& t, GlobalTransform& gt, Relation& r)
		{
			if (gt.m_dirty)
				UpdateTransforms(t, gt, r.GetParent());
		});

	m_dirty.SetPriority(100000.0f);
	m_dirty_descendants.SetPriority(90000.0f);
	m_update_global.SetPriority(0.0f);
}

bool TransformSystem::IsRequired() const noexcept
{
	return true;
}

void TransformSystem::SetGlobalPosition(const EntityID entity, const Vector2f& position) 
{
	auto global		= CheckCache(entity).Get<Transform>();
	auto relation	= m_entity_admin->TryGetComponent<Relation>(entity);

	if (relation.has_value())
	{
		SetGlobalPosition(*global, *relation.value(), position);
	}
	else // no relation component, set position directly
	{
		global->SetPosition(position);
	}
}
void TransformSystem::SetGlobalScale(const EntityID entity, const Vector2f& scale)
{

}
void TransformSystem::SetGlobalRotation(const EntityID entity, const sf::Angle angle)
{

}

void TransformSystem::SetGlobalPosition(Transform& transform, Relation& relation, const Vector2f& position)
{
	if (relation.HasParent())
	{
		GlobalTransform& parent = *CheckCache(relation.GetParent().GetEntityID()).Get<GlobalTransform>();
		transform.SetPosition(parent.GetInverseTransform() * position); // global_transform position to parent space to appear global
	}
	else transform.SetPosition(position);
}
void TransformSystem::SetGlobalScale(Transform& transform, Relation& relation, const Vector2f& scale)
{
	// TODO: implement
}
void TransformSystem::SetGlobalRotation(Transform& transform, Relation& relation, const sf::Angle angle)
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

void TransformSystem::DirtyDescendants(GlobalTransform& global_transform, const Relation::Children& children) const
{
	const auto RecursiveClean = [this](GlobalTransform& child_transform, const Relation::Children& children)
	{
		auto RecursiveImpl = [this](GlobalTransform& child_transform, const Relation::Children& children, auto& recursive_ref) mutable -> void
		{
			child_transform.m_dirty = true; // all of the children needs their global global_transform to be updated
			for (const auto& ref : children)
			{
				GlobalTransform* child_transform = CheckCache(ref.GetEntityID()).Get<GlobalTransform>();
				if (child_transform != nullptr && !child_transform->m_dirty)
					recursive_ref(*child_transform, ref->GetChildren(), recursive_ref);
			}
		};

		return RecursiveImpl(child_transform, children, RecursiveImpl);
	};

	RecursiveClean(global_transform, children);
}
void TransformSystem::UpdateTransforms(Transform& transform, GlobalTransform& global_transform, const Relation::Parent& parent) const
{
	if (parent.IsValid())
	{
		CacheSet& parent_set = CheckCache(parent.GetEntityID());

		GlobalTransform* parent_transform = parent_set.Get<GlobalTransform>();

		if (parent_transform->m_dirty) // only update if not up-to-date
			UpdateTransforms(*parent_set.Get<Transform>(), *parent_transform, parent->GetParent());

		// update global_transform values

		const Mat4f new_transform = parent_transform->GetTransform() * transform.GetTransform();
		if (new_transform != global_transform.m_transform)
		{
			global_transform.m_transform		= new_transform;
			global_transform.m_update_inverse	= true;

			global_transform.m_update_position	= true;
			global_transform.m_update_scale		= true;
			global_transform.m_update_rotation	= true;
		}
	}
	else UpdateToLocal(transform, global_transform);

	global_transform.m_dirty = false;
}

void TransformSystem::UpdateToLocal(Transform& transform, GlobalTransform& global_transform) const
{
	global_transform.m_transform		= transform.GetTransform();
	global_transform.m_update_inverse	= true;

	global_transform.m_position	= transform.m_position - transform.m_origin;
	global_transform.m_scale	= transform.m_scale;
	global_transform.m_rotation	= transform.m_rotation;
}

auto TransformSystem::CheckCache(EntityID entity_id) const -> CacheSet&
{
	const auto it = m_cache.find(entity_id);
	if (it == m_cache.end())
	{
		return m_cache.emplace(entity_id, 
			m_entity_admin->GetComponentsRef<Transform, GlobalTransform>(entity_id)).first->second; // stupid intellisense error
	}

	return it->second;
}
