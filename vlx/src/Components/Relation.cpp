#include <Velox/Components/Relation.h>

using namespace vlx;

Relation::~Relation()
{

}

void Relation::AttachParent(const EntityAdmin& entity_admin, Relation& parent, const EntityID entity_id, bool invalidate_children)
{
	parent.AttachChild(entity_admin, *this, entity_id, invalidate_children);
}
void Relation::DetachParent(const EntityAdmin& entity_admin, Relation& parent, const EntityID entity_id, bool invalidate_children)
{
	parent.DetachChild(entity_admin, *this, entity_id, invalidate_children);
}

void Relation::AttachChild(const EntityAdmin& entity_admin, Relation& child, const EntityID entity_id, bool invalidate_children)
{

}
void Relation::DetachChild(const EntityAdmin& entity_admin, Relation& child, const EntityID entity_id, bool invalidate_children)
{

}