#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.h>

#include <Velox/World/SystemObject.h>

namespace vlx
{
	class VELOX_API TransformSystem : public SystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

	private:
		using LocalSystem = System<Transform>;
		using GlobalSystem = System<Transform, Relation>;

	public:
		TransformSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void SetGlobalPosition(	const EntityID entity, const sf::Vector2f& position);
		void SetGlobalScale(	const EntityID entity, const sf::Vector2f& scale);
		void SetGlobalRotation(	const EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	Transform& transform, Relation& relation, const sf::Vector2f& position);
		void SetGlobalScale(	Transform& transform, Relation& relation, const sf::Vector2f& scale);
		void SetGlobalRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

	public:
		void PreUpdate() override;
		void Update() override;
		void PostUpdate() override;

	private:
		void CleanTransforms(Transform& transform, const Relation& relation) const;
		void UpdateTransforms(Transform& transform, const Relation& relation) const;

		void UpdateLocalTransform(Transform& transform) const;

		Transform* CheckCache(const EntityID entity_id) const;

	private:
		LocalSystem				m_local_system;
		GlobalSystem			m_global_system;
		GlobalSystem			m_cleaning_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
		
		mutable std::unordered_map<EntityID, ComponentRefPtr<Transform>> m_cache;
	};
}