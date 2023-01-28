#include <Velox/Components/Relation.h>

using namespace vlx;

bool Relation::HasParent() const noexcept
{
	return m_parent != nullptr;
}

constexpr bool Relation::HasChildren() const noexcept
{
	return !m_children.empty();
}

auto Relation::GetParent() const noexcept -> const RelationPtr
{
	return m_parent;
}
auto Relation::GetChildren() const noexcept -> const Children&
{
	return m_children;
}

void Relation::Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	// should relation really be copyable in a one-way tree?

	if (HasParent())
	{
		Relation& parent = **m_parent;

		parent.m_children.push_back(entity_admin.GetComponentRef<Relation>(entity_id, this));
		parent.PropagateAttach(entity_admin, entity_id, *this);
	}

	m_children.clear(); // to prevent children confusing who their parent is
}

void Relation::Modified(const EntityAdmin& entity_admin, const EntityID entity_id, const IComponent& new_data)
{
	// When the relation is modified, we need to update the relation accordingly

	Destroyed(entity_admin, entity_id); // detach everything first

	// then attach everything with the new values

	const Relation& new_relation = static_cast<const Relation&>(new_data);

	if (new_relation.HasParent())
		(*new_relation.m_parent)->AttachChild(entity_admin, new_relation.m_parent->GetEntityID(), entity_id, *this);

	for (const auto& child : new_relation.m_children)
		AttachChild(entity_admin, entity_id, child->GetEntityID(), **child);
}

void Relation::Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	// When the relation is destroyed, we need to detach it accordingly

	if (HasParent())
		(*m_parent)->DetachChild(entity_admin, m_parent->GetEntityID(), entity_id, *this);

	for (const auto& child : m_children)
		DetachChild(entity_admin, entity_id, child->GetEntityID(), **child);
}

void Relation::AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	if (m_descendants.contains(child_id))
		throw std::runtime_error("The new parent cannot be a descendant of the child");

	if (child.HasParent() && child.m_parent->GetEntityID() == entity_id) // child is already correctly parented
		return;

	if (HasParent() && m_parent->GetEntityID() == child_id) // special case
		(*m_parent)->DetachChild(entity_admin, m_parent->GetEntityID(), entity_id, *this);

	if (child.HasParent()) // if child already has an attached parent we need to detach it
		(*child.m_parent)->DetachChild(entity_admin, child.m_parent->GetEntityID(), child_id, child);

	child.m_parent = entity_admin.GetComponentRef<Relation>(entity_id, this);
	m_children.push_back(entity_admin.GetComponentRef<Relation>(child_id, &child));

	PropagateAttach(entity_admin, child_id, child);
}

EntityID Relation::DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child)
{
	auto found = std::find_if(m_children.begin(), m_children.end(), 
		[&child_id](const RelationPtr& ptr)
		{
			return ptr->GetEntityID() == child_id;
		});

	if (found == m_children.end())
		return NULL_ENTITY;

	PropagateDetach(entity_admin, child_id, child);

	child.m_parent.reset();

	*found = m_children.back(); // TODO: uncertain if works
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
		(*m_parent)->PropagateAttach(entity_admin, child_id, child);
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
		(*m_parent)->PropagateDetach(entity_admin, child_id, child);
}