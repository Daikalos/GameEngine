#include <Velox/World/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), 
	m_local_system(entity_admin, LYR_TRANSFORM), 
	m_global_system(entity_admin, LYR_TRANSFORM), 
	m_cleaning_system(entity_admin, LYR_TRANSFORM)
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
	m_global_system.SetPriority(1.0f);
	m_cleaning_system.SetPriority(2.0f);	// cleaning system runs before global
}

constexpr LayerType vlx::TransformSystem::GetID() const noexcept
{
	return LYR_TRANSFORM;
}

void TransformSystem::SetGlobalPosition(const EntityID entity, const sf::Vector2f& position) 
{
	SetGlobalPosition(m_entity_admin->GetComponent<Transform>(entity), 
		m_entity_admin->GetComponent<Relation>(entity), position);
}
void TransformSystem::SetGlobalScale(const EntityID entity, const sf::Vector2f& scale)
{
	SetGlobalScale(m_entity_admin->GetComponent<Transform>(entity),
		m_entity_admin->GetComponent<Relation>(entity), scale);
}
void TransformSystem::SetGlobalRotation(const EntityID entity, const sf::Angle angle)
{
	SetGlobalRotation(m_entity_admin->GetComponent<Transform>(entity),
		m_entity_admin->GetComponent<Relation>(entity), angle);
}

void TransformSystem::SetGlobalPosition(Transform& transform, Relation& relation, const sf::Vector2f& position)
{
	Transform& parent = m_entity_admin->GetComponent<Transform>(relation.GetParent());
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

void TransformSystem::Update()
{
	m_entity_admin->RunSystems(LYR_TRANSFORM);
}

void TransformSystem::CleanTransforms(Transform& transform, const Relation& relation) const
{
	if (transform.m_update_model) // no need to update if already cleaned
		return;

	transform.m_update_model = true;
	transform.m_update_inverse_model = true;

	transform.m_dirty = false;

	for (const EntityID child_id : relation.GetChildren()) // all of the children needs their global transform to be updated
	{
		CleanTransforms(m_entity_admin->GetComponent<Transform>(child_id), 
			m_entity_admin->GetComponent<Relation>(child_id));
	}
}
void TransformSystem::UpdateTransforms(Transform& transform, const Relation& relation) const
{
	if (!transform.m_update_model) // already up-to-date
		return;

	if (relation.HasParent())
	{
		Transform& parent_transform = m_entity_admin->GetComponent<Transform>(relation.GetParent()); // TODO: find way to replace GetComponent, maybe use ComponentRef
		const Relation& parent_relation = m_entity_admin->GetComponent<Relation>(relation.GetParent());

		UpdateTransforms(parent_transform, parent_relation);
		transform.ComputeTransform(parent_transform.GetTransform());

		const float* matrix = transform.m_model_transform.getMatrix();

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

	transform.m_update_model = false;
}

void TransformSystem::UpdateLocalTransform(Transform& transform) const
{
	transform.ComputeTransform();

	transform.m_global_position = transform.m_position;
	transform.m_global_scale = transform.m_scale;
	transform.m_global_rotation = transform.m_rotation;
}
