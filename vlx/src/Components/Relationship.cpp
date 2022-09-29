#include <Velox/Components/Relationship.h>

using namespace vlx;

Relationship::~Relationship()
{

}

void Relationship::AttachParent(const EntityAdmin& entity_admin, Relationship& parent, const EntityID entity_id) 
{
	parent.AttachChild(entity_admin, *this, entity_id);
}
void Relationship::DetachParent(const EntityAdmin& entity_admin, Relationship& parent, const EntityID entity_id)
{
	parent.DetachChild(entity_admin, *this, entity_id);
}

void Relationship::AttachChild(const EntityAdmin& entity_admin, Relationship& child, const EntityID entity_id)
{

}
void Relationship::DetachChild(const EntityAdmin& entity_admin, Relationship& child, const EntityID entity_id)
{

}