#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin& entity_admin)
	: m_entity_admin(&entity_admin), m_system(entity_admin, LYR_TRANSFORM)
{
	m_system.Action([this](std::span<const EntityID> entities, Transform* transforms, Relation* relations)
		{
			Update();

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				
			}
		});
}

void TransformSystem::AttachInstant(const EntityID parent, const EntityID child)
{
	AttachChild(
		m_entity_admin->GetComponent<Relation>(parent), 
		m_entity_admin->GetComponent<Relation>(child));
}
void TransformSystem::DetachInstant(const EntityID parent, const EntityID child)
{
	DetachChild(
		m_entity_admin->GetComponent<Relation>(parent),
		m_entity_admin->GetComponent<Relation>(child));
}

void TransformSystem::AttachDelay(const EntityID parent, const EntityID child)
{
	m_attachments.push(std::make_pair(parent, child));
}
void TransformSystem::DetachDelay(const EntityID parent, const EntityID child)
{
	m_detachments.push(std::make_pair(parent, child));
}

void TransformSystem::AttachChild(Relation& parent, Relation& child)
{

}
void TransformSystem::DetachChild(Relation& parent, Relation& child)
{

}

void TransformSystem::Update()
{
	while (!m_detachments.empty())
	{
		auto& pair = m_detachments.front();

		const EntityID parent = pair.first;
		const EntityID child = pair.second;

		DetachChild(
			m_entity_admin->GetComponent<Relation>(pair.first),
			m_entity_admin->GetComponent<Relation>(pair.second));

		m_detachments.pop();
	}

	while (!m_attachments.empty())
	{
		auto& pair = m_attachments.front();

		AttachChild(
			m_entity_admin->GetComponent<Relation>(pair.first),
			m_entity_admin->GetComponent<Relation>(pair.second));

		m_attachments.pop();
	}
}