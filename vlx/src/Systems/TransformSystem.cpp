#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_TRANSFORM)
{
	m_system.Action([this](std::span<const EntityID> entities, Transform* transforms)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				
			}
		});
}

void TransformSystem::SetPositionGlobal(const EntityID entity_id, const sf::Vector2f& position)
{
	Transform& transform = m_entity_admin->GetComponent<Transform>(entity_id);


}

void TransformSystem::AttachInstant(const EntityID parent, const EntityID child)
{
	AttachChild(parent, child);
}
void TransformSystem::DetachInstant(const EntityID parent, const EntityID child)
{
	DetachChild(parent, child);
}

void TransformSystem::AttachDelay(const EntityID parent, const EntityID child)
{
	if (parent != child)
		m_attachments.push(std::make_pair(parent, child));
}
void TransformSystem::DetachDelay(const EntityID parent, const EntityID child)
{
	if (parent != child)
		m_detachments.push(std::make_pair(parent, child));
}

void TransformSystem::AttachChild(const EntityID parent_id, const EntityID child_id)
{
	Transform& parent = m_entity_admin->GetComponent<Transform>(parent_id);
	Transform& child = m_entity_admin->GetComponent<Transform>(child_id);

	parent.AttachChild(*m_entity_admin, parent_id, child_id, child);
}
void TransformSystem::DetachChild(const EntityID parent_id, const EntityID child_id)
{
	Transform& parent = m_entity_admin->GetComponent<Transform>(parent_id);
	Transform& child = m_entity_admin->GetComponent<Transform>(child_id);

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