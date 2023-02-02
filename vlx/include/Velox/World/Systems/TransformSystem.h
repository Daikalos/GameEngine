#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/LocalTransform.h>
#include <Velox/Components/Transform.h>
#include <Velox/Components/Relation.h>

#include <Velox/World/SystemObject.h>

namespace vlx
{
	class VELOX_API TransformSystem : public SystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;
		using TransformSet = ComponentSet<LocalTransform, Transform>;

	private:
		using LocalSystem = System<LocalTransform, Transform>;
		using GlobalSystem = System<LocalTransform, Transform, Relation>;

	public:
		TransformSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void SetGlobalPosition(	const EntityID entity, const sf::Vector2f& position);
		void SetGlobalScale(	const EntityID entity, const sf::Vector2f& scale);
		void SetGlobalRotation(	const EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	LocalTransform& transform, Relation& relation, const sf::Vector2f& position);
		void SetGlobalScale(	LocalTransform& transform, Relation& relation, const sf::Vector2f& scale);
		void SetGlobalRotation(	LocalTransform& transform, Relation& relation, const sf::Angle angle);

	public:
		void PreUpdate() override;
		void Update() override;
		void PostUpdate() override;

	private:
		void CleanTransforms(Transform& transform, const Relation& relation) const;
		void UpdateTransforms(LocalTransform& local_transform, Transform& transform, const Relation& relation) const;

		void UpdateToLocal(LocalTransform& local_transform, Transform& transform) const;

		auto CheckCache(EntityID entity_id) const -> TransformSet&;

	private:
		LocalSystem				m_local_system;
		LocalSystem				m_dirty_system;
		GlobalSystem			m_cleaning_system;
		GlobalSystem			m_global_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
		
		mutable std::unordered_map<EntityID, TransformSet> m_cache;
	};
}