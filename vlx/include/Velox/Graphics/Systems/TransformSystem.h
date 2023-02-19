#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Relation.h>

#include <Velox/Physics/PhysicsBody.h>

#include <Velox/ECS/SystemObject.h>

namespace vlx
{
	class VELOX_API TransformSystem : public SystemObject
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;
		using TransformSet = ComponentSet<LocalTransform, Transform>;

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
		void DirtyChildrenTransform(Transform& transform, const Relation::Children& children) const;
		void UpdateTransforms(LocalTransform& local_transform, Transform& transform, const Relation::Parent& parent) const;

		void UpdateToLocal(LocalTransform& local_transform, Transform& transform) const;

		auto CheckCache(EntityID entity_id) const -> TransformSet&;

	private:
		System<LocalTransform, Transform, Relation>	m_dirty_system;
		System<Transform, Relation>					m_dirty_children_system;
		System<LocalTransform, Transform, Relation>	m_global_system;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
		
		mutable std::unordered_map<EntityID, TransformSet> m_cache;
	};
}