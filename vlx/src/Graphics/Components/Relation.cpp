#include <Velox/Graphics/Components/Relation.h>

using namespace vlx;

bool Relation::HasParent() const noexcept
{
	return m_parent.ptr.IsValid() && m_parent.entity_id != NULL_ENTITY;
}

bool Relation::HasChildren() const noexcept
{
	return !m_children.empty();
}

auto Relation::GetParent() const noexcept -> const Parent&
{
	return m_parent;
}
auto Relation::GetChildren() const noexcept -> const Children&
{
	return m_children;
}

bool Relation::IsDescendant(EntityID descendant) const
{
	 for (const auto& ref : m_children)
	 {
		 if (ref.entity_id == descendant)
			 return true;

		 if (ref.ptr->IsDescendant(descendant))
			 return true;
	 }

	 return false;
}

void Relation::CopiedImpl(const EntityAdmin& entity_admin, EntityID entity_id)
{
	if (HasParent())
	{
		Relation& parent = *m_parent.ptr;
		parent.m_children.emplace_back(entity_admin.GetComponentRef<Relation>(entity_id, this), entity_id);
	}

	m_children.clear(); // to prevent children confusing who their parent is
}

void Relation::AlteredImpl(const EntityAdmin& entity_admin, EntityID entity_id, Relation& new_data)
{
	// When the relation is modified, we need to update the relation accordingly

	DestroyedImpl(entity_admin, entity_id); // detach everything first

	// then attach everything with the new values

	if (new_data.HasParent())
		new_data.m_parent.ptr->m_children.emplace_back(entity_admin.GetComponentRef<Relation>(entity_id, this), entity_id);

	for (auto& child : new_data.m_children)
	{
		child.ptr->m_parent.ptr = entity_admin.GetComponentRef<Relation>(entity_id, this);
		child.ptr->m_parent.entity_id = entity_id;
	}
}

void Relation::DestroyedImpl(const EntityAdmin& entity_admin, EntityID entity_id)
{
	// When the relation is destroyed, we need to detach it accordingly

	if (HasParent())
	{
		cu::Erase(m_parent.ptr->m_children, 
			[&entity_id](const Ref& ref)
			{
				return ref.entity_id == entity_id;
			});
	}

	for (auto& child : m_children)
	{
		child.ptr->m_parent.ptr.Reset();
		child.ptr->m_parent.entity_id = NULL_ENTITY;
	}
}