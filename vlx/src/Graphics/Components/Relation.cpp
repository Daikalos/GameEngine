#include <Velox/Graphics/Components/Relation.h>

using namespace vlx;

bool Relation::HasParent() const noexcept
{
	return m_parent.IsValid();
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

bool Relation::IsDescendant(const EntityID descendant) const
{
	 for (const auto& ptr : m_children)
	 {
		 if (ptr.GetEntityID() == descendant)
			 return true;

		 if (ptr->IsDescendant(descendant))
			 return true;
	 }

	 return false;
}

void Relation::CopiedImpl(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	if (HasParent())
	{
		Relation& parent = *m_parent;
		parent.m_children.emplace_back(entity_admin.GetComponentRef<Relation>(entity_id, this));
	}

	m_children.clear(); // to prevent children confusing who their parent is
}

void Relation::ModifiedImpl(const EntityAdmin& entity_admin, const EntityID entity_id, Relation& new_data)
{
	// When the relation is modified, we need to update the relation accordingly

	DestroyedImpl(entity_admin, entity_id); // detach everything first

	// then attach everything with the new values

	if (new_data.HasParent())
		new_data.m_parent->m_children.push_back(entity_admin.GetComponentRef<Relation>(entity_id, this));

	for (auto& child : new_data.m_children)
		child->m_parent = entity_admin.GetComponentRef<Relation>(entity_id, this);
}

void Relation::DestroyedImpl(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	// When the relation is destroyed, we need to detach it accordingly

	if (HasParent())
	{
		Relation& parent_relation = *m_parent;
		cu::SwapPop(parent_relation.m_children,
			[&entity_id](const Ref& ref)
			{
				return ref.GetEntityID() == entity_id;
			});
	}

	for (auto& child : m_children)
		child->m_parent.Reset();
}