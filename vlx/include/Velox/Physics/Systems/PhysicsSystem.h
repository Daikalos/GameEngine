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

#include "NarrowSystem.h"

namespace vlx
{
	class VELOX_API PhysicsSystem final : public SystemAction
	{
	public:
		PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time, NarrowSystem& narrow_system);

	public:
		bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	public:
		const Vector2f& GetGravity() const;
		void SetGravity(const Vector2f& gravity);

	private:
		void Initialize(CollisionData& data);
		void ResolveCollision(CollisionData& data);
		void PositionalCorrection(CollisionData& data);

	private:
		Time*			m_time			{nullptr};
		NarrowSystem*	m_narrow_system {nullptr};

		Vector2f		m_gravity		{0.0f, 9.82f};

		System<PhysicsBody>					m_update_forces;
		System<PhysicsBody, LocalTransform>	m_update_positions;
		System<PhysicsBody>					m_clear_forces;
		System<PhysicsBody>					m_sleep_bodies;
	};
}