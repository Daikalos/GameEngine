#include <Velox/Components/Relation.h>

using namespace vlx;

void Relation::Created(const EntityAdmin& entity_admin, const EntityID entity_id)
{

}

void Relation::Moved(const EntityAdmin& entity_admin, const EntityID entity_id)
{

}

void Relation::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	if (m_parent != NULL_ENTITY)
		entity_admin.GetComponent<Relation>(m_parent).DetachChild(entity_admin, m_parent, entity_id, *this);

	for (const EntityID& child : m_children)
		entity_admin.GetComponent<Relation>(child).m_parent = NULL_ENTITY;
}

void Relation::AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	assert(entity_id != NULL_ENTITY && child_id != NULL_ENTITY);

	if (child.m_parent == entity_id) // this is already correctly parented
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
}

const EntityID Relation::DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	auto found = std::find(m_children.begin(), m_children.end(), child_id);

	if (found == m_children.end())
		return NULL_ENTITY;

	child.m_parent = NULL_ENTITY;
	m_children.erase(found);
}