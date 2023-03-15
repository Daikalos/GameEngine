#pragma once

#include <vector>
#include <span>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/System/Time.h>
#include <Velox/Config.hpp>

#include "../CollisionObject.h"

namespace vlx
{
	class VELOX_API PhysicsSystem final : public SystemAction
	{
	private:
		using CollisionPair = std::pair<CollisionObject, CollisionObject>;

	public:
		PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time);

	public:
		constexpr bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	public:
		void SupplyBroadCollisions(
			std::span<const CollisionPair> collision_pairs,
			std::span<const std::uint32_t> unique_collisions);

		//void SupplyNarrowCollisions(
		//	std::span<const CollisionData
		//)

	private:
		//void Initialize(CollisionData& data);
		//void ResolveCollision(CollisionData& data);
		//void PositionalCorrection(CollisionData& data);

	private:
		Time*		m_time		{nullptr};
		Vector2f	m_gravity	{0.0f, 10.0f};

		std::span<const CollisionPair> m_collision_pairs;
		std::span<const std::uint32_t> m_unique_collisions;

		//System<PhysicsBody>					m_update_forces;
		//System<PhysicsBody, LocalTransform>	m_update_velocities;
		//System<PhysicsBody>					m_clear_forces;
	};
}