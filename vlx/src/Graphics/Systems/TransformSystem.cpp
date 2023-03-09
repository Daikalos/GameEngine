#include <Velox/Graphics/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemAction(entity_admin, id), 
	m_dirty(entity_admin, id),
	m_dirty_descendants(entity_admin, id),
	m_update_global(entity_admin, id)
{
	m_dirty.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform)
		{
			if (local_transform.m_dirty) // if local is dirty, so is global transform
			{
				transform.m_dirty = true;
				local_transform.m_dirty = false;
			}
		});

	m_dirty_descendants.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform, Relation& relation)
		{
			if (transform.m_dirty)
			{
				DirtyDescendants(transform, relation.GetChildren());

				if (!relation.HasParent()) // can already update to local if the entity does not have any parent anyways
				{
					UpdateToLocal(local_transform, transform);
					transform.m_dirty = false;
				}
			}
		});

	m_update_global.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform, Relation& relation)
		{
			if (transform.m_dirty)
				UpdateTransforms(local_transform, transform, relation.GetParent());
		});

	m_dirty.SetPriority(100000.0f);
	m_dirty_descendants.SetPriority(99999.0f);
	m_update_global.SetPriority(0.0f);

	m_update_global.Exclude<PhysicsBody>();
}

constexpr bool TransformSystem::IsRequired() const noexcept
{
	return true;
}

void TransformSystem::SetGlobalPosition(const EntityID entity, const Vector2f& position) 
{
	SetGlobalPosition(m_entity_admin->GetComponent<LocalTransform>(entity),
		m_entity_admin->GetComponent<Relation>(entity), position);
}
void TransformSystem::SetGlobalScale(const EntityID entity, const Vector2f& scale)
{
	SetGlobalScale(m_entity_admin->GetComponent<LocalTransform>(entity), 
		m_entity_admin->GetComponent<Relation>(entity), scale);
}
void TransformSystem::SetGlobalRotation(const EntityID entity, const sf::Angle angle)
{
	SetGlobalRotation(m_entity_admin->GetComponent<LocalTransform>(entity), 
		m_entity_admin->GetComponent<Relation>(entity), angle);
}

void TransformSystem::SetGlobalPosition(LocalTransform& transform, Relation& relation, const Vector2f& position)
{
	if (relation.HasParent())
	{
		Transform& parent = *CheckCache(relation.GetParent().GetEntityID()).Get<Transform>();
		transform.SetPosition(parent.GetInverseTransform() * position);
	}
	else transform.SetPosition(position);
}
void TransformSystem::SetGlobalScale(LocalTransform& transform, Relation& relation, const Vector2f& scale)
{
	// TODO: implement
}
void TransformSystem::SetGlobalRotation(LocalTransform& transform, Relation& relation, const sf::Angle angle)
{
	// TODO: implement
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

void TransformSystem::DirtyDescendants(Transform& transform, const Relation::Children& children) const
{
	const auto RecursiveClean = [this](Transform& child_transform, const Relation::Children& children)
	{
		auto RecursiveImpl = [this](Transform& child_transform, const Relation::Children& children, auto& recursive_ref) mutable -> void
		{
			child_transform.m_dirty = true; // all of the children needs their global transform to be updated
			for (const auto& ref : children)
			{
				Transform* child_transform = CheckCache(ref.GetEntityID()).Get<Transform>();
				if (child_transform != nullptr && !child_transform->m_dirty)
					recursive_ref(*child_transform, ref->GetChildren(), recursive_ref);
			}
		};

		return RecursiveImpl(child_transform, children, RecursiveImpl);
	};

	RecursiveClean(transform, children);
}
void TransformSystem::UpdateTransforms(LocalTransform& local_transform, Transform& transform, const Relation::Parent& parent) const
{
	if (parent.IsValid())
	{
		auto& parent_set = CheckCache(parent.GetEntityID());

		Transform* parent_transform = parent_set.Get<Transform>();

		if (parent_transform->m_dirty) // only update if not up-to-date
			UpdateTransforms(*parent_set.Get<LocalTransform>(), *parent_transform, parent->GetParent());

		// update transform values

		const sf::Transform new_transform = parent_transform->GetTransform() * local_transform.GetTransform();
		if (new_transform != transform.m_transform)
		{
			transform.m_transform		= new_transform;
			transform.m_update_inverse	= true;

			transform.m_update_position = true;
			transform.m_update_scale	= true;
			transform.m_update_rotation = true;
		}
	}

	transform.m_dirty = false;
}

void TransformSystem::UpdateToLocal(LocalTransform& local_transform, Transform& transform) const
{
	transform.m_transform		= local_transform.GetTransform();
	transform.m_update_inverse	= true;

	transform.m_position	= local_transform.m_position;
	transform.m_scale		= local_transform.m_scale;
	transform.m_rotation	= local_transform.m_rotation;
}

auto vlx::TransformSystem::CheckCache(EntityID entity_id) const -> TransformSet&
{
	const auto it = m_cache.find(entity_id);
	if (it == m_cache.end())
	{
		return m_cache.try_emplace(entity_id, 
			m_entity_admin->GetComponentsRef<LocalTransform, Transform>(entity_id)).first->second; // stupid intellisense error
	}

	return it->second;
}
