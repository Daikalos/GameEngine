#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.h>

#include "ISystemObject.h"

namespace vlx
{
	class VELOX_API TransformSystem : public ISystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

	private:
		using GlobalSystem = System<Transform, Relation>;

	public:
		TransformSystem(EntityAdmin& entity_admin);

		void SetGlobalPosition(	const EntityID entity, const sf::Vector2f& position);
		void SetGlobalScale(	const EntityID entity, const sf::Vector2f& scale);
		void SetGlobalRotation(	const EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	Transform& transform, Relation& relation, const sf::Vector2f& position);
		void SetGlobalScale(	Transform& transform, Relation& relation, const sf::Vector2f& scale);
		void SetGlobalRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

		void Update() override;

	private:
		EntityAdmin*			m_entity_admin{nullptr};

		GlobalSystem			m_global_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
	};
}