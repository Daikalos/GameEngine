#include <Velox/World/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin, const LayerType id)
	: SystemObject(entity_admin, id), 
	m_local_system(entity_admin, id),
	m_global_system(entity_admin, id),
	m_cleaning_system(entity_admin, id)
{
	m_local_system.Action([this](std::span<const EntityID> entities, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Transform& transform = transforms[i];

				if (transform.m_dirty)
				{
					UpdateLocalTransform(transform);
					transform.m_dirty = false;
				}
			}
		});

	m_global_system.Action([this](std::span<const EntityID> entities, Transform* transforms, Relation* relations)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
				UpdateTransforms(transforms[i], relations[i]);
		});

	m_cleaning_system.Action([this](std::span<const EntityID> entities, Transform* transforms, Relation* relations)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (transforms[i].m_dirty)
					CleanTransforms(transforms[i], relations[i]);
			}
		});

	m_local_system.Exclude<Relation>();		// runs on any entity that does not have a relation

	m_local_system.SetPriority(0.0f);
	m_global_system.SetPriority(1.0f);
	m_cleaning_system.SetPriority(2.0f);	// cleaning system runs before global

	m_local_system.RunParallel(true);
}

void TransformSystem::SetGlobalPosition(const EntityID entity, const sf::Vector2f& position) 
{
	SetGlobalPosition(*CheckCache(entity), m_entity_admin->GetComponent<Relation>(entity), position);
}
void TransformSystem::SetGlobalScale(const EntityID entity, const sf::Vector2f& scale)
{
	SetGlobalScale(*CheckCache(entity), m_entity_admin->GetComponent<Relation>(entity), scale);
}
void TransformSystem::SetGlobalRotation(const EntityID entity, const sf::Angle angle)
{
	SetGlobalRotation(*CheckCache(entity), m_entity_admin->GetComponent<Relation>(entity), angle);
}

void TransformSystem::SetGlobalPosition(Transform& transform, Relation& relation, const sf::Vector2f& position)
{
	Transform& parent = m_entity_admin->GetComponent<Transform>(relation.GetParent()->GetEntityID());
	transform.SetPosition(parent.GetInverseTransform() * position);
}
void TransformSystem::SetGlobalScale(Transform& transform, Relation& relation, const sf::Vector2f& scale)
{
	// TODO: implement
}
void TransformSystem::SetGlobalRotation(Transform& transform, Relation& relation, const sf::Angle angle)
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
	if (transform.m_update_global) // no need to update if already cleaned
		return;

	transform.m_update_global = true;
	transform.m_update_global_inverse = true;

	transform.m_dirty = false;

	for (const auto& ptr : relation.GetChildren()) // all of the children needs their global transform to be updated
	{
		Transform* child_transform = CheckCache(ptr->GetEntityID());

		if (child_transform == nullptr)
			continue;

		CleanTransforms(*child_transform, **ptr);
	}
}
void TransformSystem::UpdateTransforms(Transform& transform, const Relation& relation) const
{
	if (!transform.m_update_global) // already up-to-date
		return;

	if (relation.HasParent())
	{
		Transform* parent_transform = CheckCache(relation.GetParent()->GetEntityID());

		if (parent_transform == nullptr)
		{
			UpdateLocalTransform(transform);
			return;
		}

		UpdateTransforms(*parent_transform, **relation.GetParent());
		transform.m_global_transform = parent_transform->GetTransform() * transform.GetLocalTransform();

		const float* matrix = transform.GetTransform().getMatrix();

		const auto mv = [&matrix](const int x, const int y) -> float
		{
			constexpr int WIDTH = 4;
			return matrix[x + y * WIDTH];
		};

		transform.m_global_position.x = mv(0, 3);
		transform.m_global_position.y = mv(1, 3);

		transform.m_global_scale.x = au::Sign(mv(0, 0)) * au::SP2(mv(0, 0), mv(1, 0));
		transform.m_global_scale.y = au::Sign(mv(1, 1)) * au::SP2(mv(0, 1), mv(1, 1));

		transform.m_global_rotation = sf::radians(std::atan2f(mv(1, 0), mv(1, 1)));
	}
	else
	{
		UpdateLocalTransform(transform);
	}

	transform.m_update_global = false;
}

void TransformSystem::UpdateLocalTransform(Transform& transform) const
{
	transform.m_global_transform = transform.GetLocalTransform();

	transform.m_global_position = transform.m_position;
	transform.m_global_scale = transform.m_scale;
	transform.m_global_rotation = transform.m_rotation;
}

Transform* vlx::TransformSystem::CheckCache(const EntityID entity_id) const
{
	const auto it = m_cache.find(entity_id);
	if (it == m_cache.end())
		return m_cache.try_emplace(entity_id, *m_entity_admin, entity_id).first->second.Get();

	return it->second.Get();
}
