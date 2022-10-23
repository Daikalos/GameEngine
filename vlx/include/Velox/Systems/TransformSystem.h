#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.h>

#include "ISystemMaster.h"

namespace vlx
{
	class VELOX_API TransformSystem : public ISystemMaster
	{
	private:
		using Attachment = std::pair<EntityID, EntityID>;

	private:
		using System = System<Relation, Transform>;

	public:
		TransformSystem(EntityAdmin& entity_admin);

		void AttachInstant(const EntityID parent, const EntityID child);
		void DetachInstant(const EntityID parent, const EntityID child);

		void AttachDelay(const EntityID parent, const EntityID child);
		void DetachDelay(const EntityID parent, const EntityID child);

		void Update() override;

	private:
		void AttachChild(const EntityID parent_id, const EntityID child_id);
		void DetachChild(const EntityID parent_id, const EntityID child_id);

		void PreUpdate() override;

	private:
		EntityAdmin*			m_entity_admin{nullptr};
		System					m_system;

		std::queue<Attachment>	m_attachments;
		std::queue<Attachment>	m_detachments;
	};
}