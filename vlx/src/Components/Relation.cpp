#include <Velox/Components/Relation.h>

using namespace vlx;

void Relation::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	if (m_parent != NULL_ENTITY)
		entity_admin.GetComponent<Relation>(m_parent).DetachChild(entity_admin, m_parent, entity_id, *this);

	for (const EntityID& child : m_children)
		entity_admin.GetComponent<Relation>(child).m_parent = NULL_ENTITY;
}

void Relation::AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	assert(!m_closed.contains(child_id));

	if (child.m_parent == entity_id) // child is already correctly parented
		return;

	if (m_parent == child_id) // special case
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.DetachChild(entity_admin, m_parent, entity_id, *this);
	}

	if (child.m_parent != NULL_ENTITY) // if child already has an attached parent we need to detach it
	{
		entity_admin.GetComponent<Relation>(child.m_parent)
			.DetachChild(entity_admin, child.m_parent, child_id, child);
	}

	child.m_parent = entity_id;
	m_children.push_back(child_id);

	PropagateAttach(entity_admin, child_id);
	child.UpdateNewRoot(entity_admin);
}

const EntityID Relation::DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	auto found = std::find(m_children.begin(), m_children.end(), child_id);

	if (found == m_children.end())
		return NULL_ENTITY;

	child.m_parent = NULL_ENTITY;
	m_children.erase(found);

	PropagateDetach(entity_admin, child_id);
	child.UpdateNewRoot(entity_admin);
}

bool Relation::HasNewRoot() const noexcept
{
	return m_new_root;
}

void Relation::PropagateAttach(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	m_closed.insert(entity_id);

	if (m_parent != NULL_ENTITY)
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.PropagateAttach(entity_admin, entity_id);
	}
}
void Relation::PropagateDetach(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	auto it = m_closed.find(entity_id);
	assert(it != m_closed.end());
	m_closed.erase(it);

	if (m_parent != NULL_ENTITY)
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.PropagateDetach(entity_admin, entity_id);
	}
}

void Relation::UpdateNewRoot(const EntityAdmin& entity_admin)
{
	m_new_root = true;

	for (const EntityID child : m_children)
	{
		entity_admin.GetComponent<Relation>(child)
			.UpdateNewRoot(entity_admin);
	}
}