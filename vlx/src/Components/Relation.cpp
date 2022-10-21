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
	//if (m_parent != NULL_ENTITY)
	//	DetachParent(*m_parent);

	//for (const EntityID& child : m_children)
	//	entity_admin.GetComponent<Relation>(child).m_parent = NULL_ENTITY;
}