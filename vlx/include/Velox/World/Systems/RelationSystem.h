#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Relation.h>

#include <Velox/World/SystemObject.h>

namespace vlx
{
	class VELOX_API RelationSystem : public SystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

	public:
		using SystemObject::SystemObject;

	public:
		void AttachInstant(const EntityID parent_id, const EntityID child_id);
		void DetachInstant(const EntityID parent_id, const EntityID child_id);

		void AttachDelay(const EntityID parent_id, const EntityID child_id);
		void DetachDelay(const EntityID parent_id, const EntityID child_id);

	public:
		void Update() override;

	private:
		void AttachChild(const EntityID parent_id, const EntityID child_id);
		void DetachChild(const EntityID parent_id, const EntityID child_id);

		void AttachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child);
		void DetachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child);

	private:
		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
	};
}