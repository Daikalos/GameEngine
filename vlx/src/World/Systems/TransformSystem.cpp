#include <Velox/World/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemObject(entity_admin, id), 
	m_local_system(entity_admin, id),
	m_dirty_system(entity_admin, id),
	m_dirty_children_system(entity_admin, id),
	m_global_system(entity_admin, id)
{
	m_local_system.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform)
		{
			transform.m_root = true;
		});

	m_dirty_system.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform)
		{
			if (local_transform.m_dirty) // if local is dirty, so is global transform
			{
				transform.m_dirty = true;
				local_transform.m_dirty = false;
			}
		});

	m_dirty_children_system.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform, Relation& relation)
		{
			if (transform.m_dirty)
				DirtyChildrenTransform(transform, relation.GetChildren());
		});

	m_global_system.Each([this](const EntityID entity, LocalTransform& local_transform, Transform& transform, Relation& relation)
		{
			if (transform.m_dirty)
				UpdateTransforms(local_transform, transform, relation.GetParent());
		});

	m_local_system.Exclude<Relation>();	// runs on any entity that does not have a parent or child
	m_local_system.RunParallel(true);	// allow multiple archetypes to run simultaneously on this system
}

void TransformSystem::SetGlobalPosition(const EntityID entity, const sf::Vector2f& position) 
{
	SetGlobalPosition(m_entity_admin->GetComponent<LocalTransform>(entity),
		m_entity_admin->GetComponent<Relation>(entity), position);
}
void TransformSystem::SetGlobalScale(const EntityID entity, const sf::Vector2f& scale)
{
	SetGlobalScale(m_entity_admin->GetComponent<LocalTransform>(entity), 
		m_entity_admin->GetComponent<Relation>(entity), scale);
}
void TransformSystem::SetGlobalRotation(const EntityID entity, const sf::Angle angle)
{
	SetGlobalRotation(m_entity_admin->GetComponent<LocalTransform>(entity), 
		m_entity_admin->GetComponent<Relation>(entity), angle);
}

void TransformSystem::SetGlobalPosition(LocalTransform& transform, Relation& relation, const sf::Vector2f& position)
{
	//Transform& parent = *CheckCache(relation.GetParent()->GetEntityID());
	//transform.SetPosition(parent.GetInverseTransform() * position);
}
void TransformSystem::SetGlobalScale(LocalTransform& transform, Relation& relation, const sf::Vector2f& scale)
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
	m_entity_admin->RunSystems(GetID());
}

void TransformSystem::PostUpdate()
{
	// TODO: figure out how to keep cache at reasonable size
}

void TransformSystem::DirtyChildrenTransform(Transform& transform, const Relation::Children& children) const
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

		sf::Transform new_transform = parent_transform->GetTransform() * local_transform.GetTransform();
		if (new_transform != transform.m_transform)
		{
			transform.m_transform = new_transform;

			transform.m_update_inverse	= true;
			transform.m_update_position = true;
			transform.m_update_scale	= true;
			transform.m_update_rotation = true;
		}

		transform.m_root = false;
	}
	else
	{
		transform.m_root = true;
	}

	transform.m_dirty = false;
}

void TransformSystem::UpdateToLocal(LocalTransform& local_transform, Transform& transform) const
{
	transform.m_transform	= local_transform.GetTransform();

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
