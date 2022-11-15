#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.hpp>

#include "ISystemObject.h"

namespace vlx
{
	class VELOX_API TransformSystem : public ISystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

	private:
		using System = System<Transform>;

	public:
		TransformSystem(EntityAdmin& entity_admin);

		void SetOrigin(		Transform& transform, const sf::Vector2f& origin);
		void SetPosition(	Transform& transform, const sf::Vector2f& position, bool global = false);
		void SetScale(		Transform& transform, const sf::Vector2f& scale);
		void SetRotation(	Transform& transform, const sf::Angle angle);

		void Move(			Transform& transform, const sf::Vector2f& move);
		void Scale(			Transform& transform, const sf::Vector2f& factor);
		void Rotate(		Transform& transform, const sf::Angle angle);

		void AttachInstant(const EntityID parent_id, const EntityID child_id);
		void DetachInstant(const EntityID parent_id, const EntityID child_id);

		void AttachInstant(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child);
		void DetachInstant(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child);

		void AttachDelay(const EntityID parent_id, const EntityID child_id);
		void DetachDelay(const EntityID parent_id, const EntityID child_id);

		void Update() override;

	private:
		void AttachChild(const EntityID parent_id, const EntityID child_id);
		void DetachChild(const EntityID parent_id, const EntityID child_id);

		void AttachChild(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child);
		void DetachChild(const EntityID parent_id, Transform& parent, const EntityID child_id, Transform& child);

		void PreUpdate();

	private:
		EntityAdmin*			m_entity_admin{nullptr};
		System					m_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
	};
}