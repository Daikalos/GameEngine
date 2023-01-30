#include <Velox/World/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemObject(entity_admin, id), 
	m_local_system(entity_admin, id),
	m_dirty_system(entity_admin, id),
	m_cleaning_system(entity_admin, id),
	m_global_system(entity_admin, id)
{
	m_local_system.Action([this](std::span<const EntityID> entities, LocalTransform* local_transforms, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				LocalTransform& local_transform = local_transforms[i];

				if (local_transform.m_dirty)
				{
					SetLocalTransform(local_transform, transforms[i]);
					local_transform.m_dirty = false;
				}
			}
		});

	m_dirty_system.Action([this](std::span<const EntityID> entities, LocalTransform* local_transforms, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				LocalTransform& local_transform = local_transforms[i];

				if (local_transform.m_dirty)
				{
					transforms[i].m_dirty = true;
					local_transform.m_dirty = false;
				}
			}
		});

	m_cleaning_system.Action([this](std::span<const EntityID> entities, LocalTransform* local_transforms, Transform* transforms, Relation* relations)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (transforms[i].m_dirty)
					CleanTransforms(transforms[i], relations[i]);
			}
		});

	m_global_system.Action([this](std::span<const EntityID> entities, LocalTransform* local_transforms, Transform* transforms, Relation* relations)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
				UpdateTransforms(local_transforms[i], transforms[i], relations[i]);
		});

	m_local_system.Exclude<Relation>();		// runs on any entity that does not have a relation

	m_dirty_system.SetPriority(3.0f);
	m_cleaning_system.SetPriority(2.0f);	// cleaning system runs before global
	m_global_system.SetPriority(1.0f);

	m_local_system.RunParallel(true);
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

void TransformSystem::CleanTransforms(Transform& transform, const Relation& relation) const
{
	const auto RecursiveClean = [this](Transform& child_transform, const Relation& child_relation) 
	{
		auto RecursiveImpl = [this](Transform& child_transform, const Relation& child_relation, auto& recursive_ref) mutable
		{
			if (child_transform.m_dirty)
				return;

			child_transform.m_dirty = true;

			for (const auto& ptr : child_relation.GetChildren()) // all of the children needs their global transform to be updated
			{
				Transform* child_transform = CheckCache(ptr.GetEntityID()).Get<Transform>();

				if (child_transform == nullptr)
					continue;

				recursive_ref(*child_transform, *ptr, recursive_ref);
			}
		};

		return RecursiveImpl(child_transform, child_relation, RecursiveImpl);
	};

	transform.m_dirty = true;

	for (const auto& ptr : relation.GetChildren()) // all of the children needs their global transform to be updated
	{
		Transform* child_transform = CheckCache(ptr.GetEntityID()).Get<Transform>();

		if (child_transform == nullptr)
			continue;

		RecursiveClean(*child_transform, *ptr);
	}
}
void TransformSystem::UpdateTransforms(LocalTransform& local_transform, Transform& transform, const Relation& relation) const
{
	if (!transform.m_dirty) // already up-to-date
		return;

	if (relation.HasParent())
	{
		auto& set = CheckCache(relation.GetParent().GetEntityID());

		Transform* parent_transform = set.Get<Transform>();

		if (parent_transform == nullptr)
		{
			SetLocalTransform(local_transform, transform);
			return;
		}

		UpdateTransforms(*set.Get<LocalTransform>(), *parent_transform, *relation.GetParent());
		transform.m_transform = parent_transform->GetTransform() * local_transform.GetTransform();

		transform.m_update_inverse = true;

		transform.m_update_position = true;
		transform.m_update_scale = true;
		transform.m_update_rotation = true;
	}
	else
	{
		SetLocalTransform(local_transform, transform);
	}

	transform.m_dirty = false;
}

void TransformSystem::SetLocalTransform(LocalTransform& local_transform, Transform& transform) const
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
			m_entity_admin->GetComponents<LocalTransform, Transform>(entity_id)).first->second; // stupid intellisense error
	}

	return it->second;
}
