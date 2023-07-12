#pragma once

#include <vector>
#include <tuple>
#include <optional>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/System/Vector2.hpp>

#include <Velox/Graphics/Components/GlobalTransformTranslation.h>
#include <Velox/Graphics/Components/GlobalTransformRotation.h>
#include <Velox/Graphics/Components/GlobalTransformScale.h>
#include <Velox/Graphics/Components/GlobalTransformDirty.h>
#include <Velox/Graphics/Components/GlobalTransformMatrix.h>
#include <Velox/Graphics/Components/GlobalTransformMatrixInverse.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/TransformMatrix.h>
#include <Velox/Graphics/Components/TransformMatrixInverse.h>
#include <Velox/Graphics/Components/Relation.h>

#include <Velox/Physics/PhysicsBody.h>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API TransformSystem final : public SystemAction
	{
	private:
		using CacheTuple = std::type_identity<std::tuple<
			TransformMatrix,
			GlobalTransformDirty, 
			GlobalTransformMatrix>>;

		using CacheSet = ComponentSet<
			TransformMatrix, 
			GlobalTransformDirty, 
			GlobalTransformMatrix>;

	public:
		TransformSystem(EntityAdmin& entity_admin, LayerType id);

	public:
		void SetGlobalPosition(	EntityID entity, const Vector2f& position);
		void SetGlobalScale(	EntityID entity, const Vector2f& scale);
		void SetGlobalRotation(	EntityID entity, const sf::Angle angle);

		void SetGlobalPosition(	Transform& transform, Relation& relation, const Vector2f& position);
		void SetGlobalScale(	Transform& transform, Relation& relation, const Vector2f& scale);
		void SetGlobalRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

	public:
		void Start() override;

		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void DirtyDescendants(
			GlobalTransformDirty& gtd, 
			const typename Relation::Children& children) const;

		void UpdateTransforms(
			TransformMatrix& tm, 
			GlobalTransformDirty& gtd, 
			GlobalTransformMatrix& gtm, 
			const typename Relation::Parent& parent) const;

		void UpdateToLocal(
			TransformMatrix& tm, 
			GlobalTransformDirty& gtd,
			GlobalTransformMatrix& gtm) const;

		auto CheckCache(EntityID entity_id) const -> std::optional<CacheSet>;

	private:
		System<
			Transform, 
			TransformMatrix>				m_sync;

		System<
			Transform, 
			GlobalTransformDirty>			m_dirty;
			
		System<
			GlobalTransformDirty, 
			Relation>						m_dirty_descendants;

		System<
			TransformMatrix,
			GlobalTransformDirty,
			GlobalTransformMatrix, 
			Relation>						m_update_global;

		System<
			GlobalTransformDirty,
			GlobalTransformMatrix,
			GlobalTransformTranslation>		m_update_pos;

		System<
			GlobalTransformDirty,
			GlobalTransformMatrix,
			GlobalTransformRotation>		m_update_rot;

		System<
			GlobalTransformDirty,
			GlobalTransformMatrix,
			GlobalTransformScale>			m_update_scl;

		mutable std::unordered_map<EntityID, CacheSet> m_cache;
	};
}