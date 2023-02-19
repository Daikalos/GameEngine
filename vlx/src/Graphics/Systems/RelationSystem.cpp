#include <Velox/Graphics/Systems/RelationSystem.h>

using namespace vlx;

void RelationSystem::Attach(const EntityID parent_id, const EntityID child_id)
{
	AttachDelay(parent_id, child_id);
}

void RelationSystem::Detach(const EntityID parent_id, const EntityID child_id)
{
	AttachDelay(parent_id, child_id);
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
	m_attachments.emplace(parent_id, child_id);
}
void RelationSystem::DetachDelay(const EntityID parent_id, const EntityID child_id)
{
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
	if (child.HasParent() && child.GetParent().GetEntityID() == parent_id) // child is already correctly parented
		return;

	if (parent_id == child_id) // cant attach to itself
		return;

	if (parent.IsDescendant(child_id))
		throw std::runtime_error("The new parent cannot be a descendant of the child");

	if (parent.HasParent() && parent.GetParent().GetEntityID() == child_id) // special case
	{
		DetachChild(parent.m_parent.GetEntityID(), *parent.m_parent, parent_id, parent);
		return;
	}

	if (child.HasParent()) // if child already has an attached parent we need to detach it
		DetachChild(child.m_parent.GetEntityID(), *child.m_parent, child_id, child);

	child.m_parent = m_entity_admin->GetComponentRef<Relation>(parent_id, &parent);
	parent.m_children.push_back(m_entity_admin->GetComponentRef<Relation>(child_id, &child));
}
EntityID RelationSystem::DetachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child)
{
	auto found = std::find_if(parent.m_children.begin(), parent.m_children.end(),
		[&child_id](const Relation::Ref& ptr)
		{
			return ptr.GetEntityID() == child_id;
		});

	if (found == parent.m_children.end())
		return NULL_ENTITY;

	child.m_parent.Reset();

	*found = parent.m_children.back();
	parent.m_children.pop_back();

	return child_id;
}
