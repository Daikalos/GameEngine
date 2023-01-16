#include <Velox/World/Systems/RelationSystem.h>

using namespace vlx;

RelationSystem::RelationSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin)
{

}

constexpr LayerType RelationSystem::GetID() const noexcept
{
	return LYR_NONE;
}

void RelationSystem::AttachInstant(const EntityID parent_id, const EntityID child_id)
{
	AttachChild(parent_id, child_id);
}
void RelationSystem::DetachInstant(const EntityID parent_id, const EntityID child_id)
{
	DetachChild(parent_id, child_id);
}

void RelationSystem::AttachDelay(const EntityID parent_id, const EntityID child_id)
{
	if (parent_id != child_id)
		m_attachments.emplace(parent_id, child_id);
}
void RelationSystem::DetachDelay(const EntityID parent_id, const EntityID child_id)
{
	if (parent_id != child_id)
		m_detachments.emplace(parent_id, child_id);
}

void RelationSystem::Update()
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

void RelationSystem::AttachChild(const EntityID parent_id, const EntityID child_id)
{
	AttachChild(parent_id, m_entity_admin->GetComponent<Relation>(parent_id),
		child_id, m_entity_admin->GetComponent<Relation>(child_id));
}
void RelationSystem::DetachChild(const EntityID parent_id, const EntityID child_id)
{
	DetachChild(parent_id, m_entity_admin->GetComponent<Relation>(parent_id),
		child_id, m_entity_admin->GetComponent<Relation>(child_id));
}

void RelationSystem::AttachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child)
{
	parent.AttachChild(*m_entity_admin, parent_id, child_id, child);
}
void RelationSystem::DetachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child)
{
	parent.DetachChild(*m_entity_admin, parent_id, child_id, child);
}
