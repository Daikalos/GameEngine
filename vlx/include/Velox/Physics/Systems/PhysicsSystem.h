#pragma once

#include <vector>
#include <span>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/System/Time.h>
#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "../Collider.h"
#include "../CollisionObject.h"
#include "../CollisionArbiter.h"
#include "../PhysicsBodyTransform.h"
#include "../PhysicsCommon.hpp"
#include "../CollisionSolver.h"

#include "BroadSystem.h"
#include "NarrowSystem.h"

namespace vlx
{
	class VELOX_API PhysicsSystem final : public SystemAction
	{
	public:
		PhysicsSystem(EntityAdmin& entity_admin, LayerType id, Time& time);

	public:
		void FixedUpdate() override;

	public:
		const Vector2f& GetGravity() const;
		void SetGravity(const Vector2f& gravity);

		void SetIterations(int iterations);

	private:
		void PreSolve(EntityID entity_id, PhysicsBodyTransform& pbt, const Transform& t) const;
		void IntegrateVelocity(EntityID entity_id, PhysicsBody& pb) const;
		void IntegratePosition(EntityID entity_id, PhysicsBody& pb, Transform& t) const;
		void SleepBodies(EntityID entity_id, PhysicsBody& pb) const;

	private:
		Time*			m_time			{nullptr};
		Vector2f		m_gravity		{0.0f, 60.82f};
		int				m_iterations	{20};

		BroadSystem		m_broad_system;
		NarrowSystem	m_narrow_system;
		CollisionSolver	m_collision_solver;

		System<PhysicsBodyTransform, const Transform> m_pre_solve;

		System<PhysicsBody>				m_integrate_velocity;
		System<PhysicsBody, Transform>	m_integrate_position;
		System<PhysicsBody>				m_sleep_bodies;
	};
}