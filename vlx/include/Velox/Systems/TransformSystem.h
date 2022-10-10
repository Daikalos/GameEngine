#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.h>

namespace vlx
{
	class VELOX_API TransformSystem
	{
	private:
		using Attachment = std::pair<EntityID, EntityID>;

	private:
		using System = System<Transform, Relation>;

	public:
		TransformSystem(EntityAdmin& entity_admin);

		void AttachChild(const EntityID parent, const EntityID child);
		void DetachChild(const EntityID parent, const EntityID child);

	private:
		void Update(Relation* relations);

	private:
		EntityAdmin* m_entity_admin;
		System m_system;

		std::queue<Attachment> m_attachments;
		std::queue<Attachment> m_detachments;
	};
}