#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Relation.h>

#include <Velox/Physics/PhysicsBody.h>

namespace vlx
{
	class VELOX_API TransformSystem final : public SystemAction
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;
		using CacheSet = ComponentSet<Transform, GlobalTransform>;

	public:
		TransformSystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		bool IsRequired() const noexcept override;

	public:
		void SetGlobalPosition(	const EntityID entity, const Vector2f& position);
		void SetGlobalScale(	const EntityID entity, const Vector2f& scale);
		void SetGlobalRotation(	const EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	Transform& transform, Relation& relation, const Vector2f& position);
		void SetGlobalScale(	Transform& transform, Relation& relation, const Vector2f& scale);
		void SetGlobalRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void DirtyDescendants(GlobalTransform& global_transform, const Relation::Children& children) const;
		void UpdateTransforms(Transform& transform, GlobalTransform& global_transform, const Relation::Parent& parent) const;

		void UpdateToLocal(Transform& transform, GlobalTransform& global_transform) const;

		auto CheckCache(EntityID entity_id) const -> CacheSet&;

	private:
		System<Transform, GlobalTransform>				m_dirty;
		System<Transform, GlobalTransform, Relation>	m_dirty_descendants;
		System<Transform, GlobalTransform, Relation>	m_update_global;

		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
		
		mutable std::unordered_map<EntityID, CacheSet> m_cache;
	};
}