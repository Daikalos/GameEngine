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
		using LocalSystem = System<Transform>;
		using GlobalSystem = System<Transform, Relation>;

	public:
		TransformSystem(EntityAdmin& entity_admin);

		[[nodiscard]] constexpr LayerType GetID() const noexcept override;

	public:
		void SetGlobalPosition(	const EntityID entity, const sf::Vector2f& position);
		void SetGlobalScale(	const EntityID entity, const sf::Vector2f& scale);
		void SetGlobalRotation(	const EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	Transform& transform, Relation& relation, const sf::Vector2f& position);
		void SetGlobalScale(	Transform& transform, Relation& relation, const sf::Vector2f& scale);
		void SetGlobalRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

	public:
		void Update() override;

	private:
		void CleanTransforms(Transform& transform, const Relation& relation) const;
		void UpdateTransforms(Transform& transform, const Relation& relation) const;

		void UpdateLocalTransform(Transform& transform) const;

	private:
		EntityAdmin*			m_entity_admin{nullptr};

		LocalSystem				m_local_system;
		GlobalSystem			m_global_system;
		GlobalSystem			m_cleaning_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
	};
}