#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_local_system(entity_admin, LYR_TRANSFORM), m_global_system(entity_admin, LYR_TRANSFORM)
{
	m_local_system.Action([this](std::span<const EntityID> entities, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (transforms[i].m_update_global)
				{
					transforms[i].UpdateRequired(*m_entity_admin, nullptr);
					transforms[i].m_update_global = false;
				}

				transforms[i].UpdateTransforms(*m_entity_admin, nullptr);
			}
		});

	m_global_system.Action([this](std::span<const EntityID> entities, Transform* transforms, Relation* relations)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (transforms[i].m_update_global)
				{
					transforms[i].UpdateRequired(*m_entity_admin, &relations[i]);
					transforms[i].m_update_global = false;
				}
			}

			for (std::size_t i = 0; i < entities.size(); ++i)
				transforms[i].UpdateTransforms(*m_entity_admin, &relations[i]);
		});

	m_local_system.Exclude<Relation>(); // runs on any entity that does not have relation
	m_local_system.SetPriority(1.0f);	// local runs before global
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

}
void TransformSystem::SetGlobalRotation(Transform& transform, Relation& relation, const sf::Angle angle)
{

}

void TransformSystem::Update()
{
	m_entity_admin->RunSystems(LYR_TRANSFORM);
}
