#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_TRANSFORM)
{
	m_system.Action([this](std::span<const EntityID> entities, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				transforms[i].UpdateTransforms(*m_entity_admin);
			}
		});
}

void TransformSystem::SetOrigin(Transform& transform, const sf::Vector2f& origin) 
{
	transform.SetOrigin(*m_entity_admin, origin);
}
void TransformSystem::SetPosition(Transform& transform, const sf::Vector2f& position, bool global) 
{
	transform.SetPosition(*m_entity_admin, position, global);
}
void TransformSystem::SetScale(Transform& transform, const sf::Vector2f& scale) 
{
	transform.SetScale(*m_entity_admin, scale);
}
void TransformSystem::SetRotation(Transform& transform, const sf::Angle angle) 
{
	transform.SetRotation(*m_entity_admin, angle);
}

void TransformSystem::Move(Transform& transform, const sf::Vector2f& move) 
{
	transform.Move(*m_entity_admin, move);
}
void TransformSystem::Scale(Transform& transform, const sf::Vector2f& factor) 
{
	transform.Scale(*m_entity_admin, factor);
}
void TransformSystem::Rotate(Transform& transform, const sf::Angle angle) 
{
	transform.Rotate(*m_entity_admin, angle);
}

void TransformSystem::AttachInstant(const EntityID parent_id, const EntityID child_id)
{
	AttachChild(parent_id, child_id);
}
void TransformSystem::DetachInstant(const EntityID parent_id, const EntityID child_id)
{
	DetachChild(parent_id, child_id);
}

void TransformSystem::AttachInstant(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child)
{
	AttachChild(parent_id, parent, child_id, child);
}
void TransformSystem::DetachInstant(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child)
{
	DetachChild(parent_id, parent, child_id, child);
}

void TransformSystem::AttachDelay(const EntityID parent_id, const EntityID child_id)
{
	if (parent_id != child_id)
		m_attachments.emplace(parent_id, child_id);
}
void TransformSystem::DetachDelay(const EntityID parent_id, const EntityID child_id)
{
	if (parent_id != child_id)
		m_detachments.emplace(parent_id, child_id);
}

void TransformSystem::AttachChild(const EntityID parent_id, const EntityID child_id)
{
	AttachChild(parent_id, m_entity_admin->GetComponent<Transform>(parent_id),
		child_id, m_entity_admin->GetComponent<Transform>(child_id));
}
void TransformSystem::DetachChild(const EntityID parent_id, const EntityID child_id)
{
	DetachChild(parent_id, m_entity_admin->GetComponent<Transform>(parent_id),
		child_id, m_entity_admin->GetComponent<Transform>(child_id));
}

void TransformSystem::AttachChild(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child)
{
	parent.AttachChild(*m_entity_admin, parent_id, child_id, child);
}
void TransformSystem::DetachChild(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child)
{
	parent.DetachChild(*m_entity_admin, parent_id, child_id, child);
}

void TransformSystem::Update()
{
	PreUpdate();
	m_entity_admin->RunSystems(LYR_TRANSFORM);
}

void TransformSystem::PreUpdate()
{
	while (!m_detachments.empty()) // detach all relations first
	{
		auto& pair = m_detachments.front();
		DetachChild(pair.first, pair.second);
		m_detachments.pop();
	}

	while (!m_attachments.empty())
	{
		auto& pair = m_attachments.front();
		AttachChild(pair.first, pair.second);
		m_attachments.pop();
	}
}