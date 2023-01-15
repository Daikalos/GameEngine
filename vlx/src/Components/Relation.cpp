#include <Velox/Components/Relation.h>

using namespace vlx;

constexpr bool Relation::HasParent() const noexcept
{
	return m_parent != NULL_ENTITY;
}

constexpr bool Relation::HasChildren() const noexcept
{
	return !m_children.empty();
}

const EntityID& Relation::GetParent() const noexcept
{
	return m_parent;
}
const std::vector<EntityID>& Relation::GetChildren() const noexcept
{
	return m_children;
}

void Relation::Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	if (HasParent())
	{
		Relation& parent = entity_admin.GetComponent<Relation>(m_parent);

		parent.m_children.emplace_back(entity_id);
		parent.PropagateAttach(entity_admin, entity_id, *this);
	}
}

void Relation::Modified(const EntityAdmin& entity_admin, const EntityID entity_id, const IComponent& new_data)
{
	// When the relation is modified, we need to update the relation accordingly

	Destroyed(entity_admin, entity_id); // detach everything first

	// then attach everything with the new values

	const Relation& new_relation = static_cast<const Relation&>(new_data);

	if (new_relation.HasParent())
	{
		entity_admin.GetComponent<Relation>(new_relation.m_parent)
			.AttachChild(entity_admin, new_relation.m_parent, entity_id, *this);
	}

	for (const EntityID child : new_relation.m_children)
	{
		AttachChild(entity_admin, entity_id, child, entity_admin.GetComponent<Relation>(child));
	}
}

void Relation::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	// When the relation is destroyed, we need to detach it accordingly

	if (HasParent())
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.DetachChild(entity_admin, m_parent, entity_id, *this);
	}

	for (const EntityID child : m_children)
	{
		DetachChild(entity_admin, entity_id, child, entity_admin.GetComponent<Relation>(child));
	}
}

void Relation::AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	if (m_descendants.contains(child_id))
		throw std::runtime_error("The new parent cannot be a descendant of the child");

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

	PropagateAttach(entity_admin, child_id, child);
}

const EntityID Relation::DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	auto found = std::find(m_children.begin(), m_children.end(), child_id);

	if (found == m_children.end())
		return NULL_ENTITY;

	PropagateDetach(entity_admin, child_id, child);

	child.m_parent = NULL_ENTITY;

	*found = m_children.back();
	m_children.pop_back();

	return child_id;
}

void Relation::PropagateAttach(const EntityAdmin& entity_admin, const EntityID child_id, const Relation& child)
{
#ifdef VELOX_DEBUG // check so that it does not exist
	assert(!m_descendants.contains(child_id));
	for (const EntityID entity : child.m_descendants)
		assert(!m_descendants.contains(entity));
#endif

	m_descendants.insert(child_id); // add child and all of its descendants
	m_descendants.insert(child.m_descendants.begin(), child.m_descendants.end());

	if (HasParent())
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.PropagateAttach(entity_admin, child_id, child);
	}
}

void Relation::PropagateDetach(const EntityAdmin& entity_admin, const EntityID child_id, const Relation& child)
{
#ifdef VELOX_DEBUG // check so that they do exist
	assert(m_descendants.contains(child_id));
	for (const EntityID entity : child.m_descendants)
		assert(m_descendants.contains(entity));
#endif

	m_descendants.erase(child_id); // remove child and all of its descendants
	m_descendants.erase(child.m_descendants.begin(), child.m_descendants.end());

	if (HasParent())
	{
		entity_admin.GetComponent<Relation>(m_parent)
			.PropagateDetach(entity_admin, child_id, child);
	}
}