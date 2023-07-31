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
	class VELOX_API GlobalTransformSystem final : public SystemAction
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

		using SyncLocalSystem			= System<Transform, TransformMatrix>;
		using DirtyLocalSystem			= System<Transform, GlobalTransformDirty>;
		using DirtyDescendantsSystem	= System<GlobalTransformDirty, Relation>;
		using UpdateGlobalSystem		= System<TransformMatrix, GlobalTransformDirty, GlobalTransformMatrix, Relation>;
		using UpdatePositionSystem		= System<GlobalTransformDirty, GlobalTransformMatrix, GlobalTransformTranslation>;
		using UpdateRotationSystem		= System<GlobalTransformDirty, GlobalTransformMatrix, GlobalTransformRotation>;
		using UpdateScaleSystem			= System<GlobalTransformDirty, GlobalTransformMatrix, GlobalTransformScale>;

	public:
		GlobalTransformSystem(EntityAdmin& entity_admin, LayerType id);

	public:
		void SetPosition(	EntityID entity, const Vector2f& position);
		void SetScale(		EntityID entity, const Vector2f& scale);
		void SetRotation(	EntityID entity, const sf::Angle angle);

		void SetPosition(	Transform& transform, Relation& relation, const Vector2f& position);
		void SetScale(		Transform& transform, Relation& relation, const Vector2f& scale);
		void SetRotation(	Transform& transform, Relation& relation, const sf::Angle angle);

	public:
		void Update() override;

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
		DirtyLocalSystem		m_dirty;
		DirtyDescendantsSystem	m_dirty_descendants;
		UpdateGlobalSystem		m_update_global;
		UpdatePositionSystem	m_update_pos;
		UpdateRotationSystem	m_update_rot;
		UpdateScaleSystem		m_update_scl;

		mutable std::unordered_map<EntityID, CacheSet> m_cache;
	};
}