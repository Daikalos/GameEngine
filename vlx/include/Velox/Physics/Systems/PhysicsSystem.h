#pragma once

#include <vector>
#include <span>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/Graphics/Components/Transform.h>

#include <Velox/System/Time.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "../PhysicsBody.h"
#include "../BodyTransform.h"
#include "../BodyLastTransform.h"
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

		void SetVelocityIterations(int iterations);
		void SetPositionIterations(int iterations);

	private:
		void IntegrateVelocity(EntityID entity_id, PhysicsBody& pb) const;
		void IntegratePosition(EntityID entity_id, PhysicsBody& pb, BodyTransform& bt) const;
		void SleepBodies(EntityID entity_id, PhysicsBody& pb) const;

		void PreSolve(EntityID entity_id, BodyTransform& pbt, BodyLastTransform& blt, const Transform& t) const;
		void PostSolve(EntityID entity_id, const BodyTransform& pbt, Transform& t) const;

	private:
		Time*			m_time			{nullptr};
		Vector2f		m_gravity		{0.0f, 60.82f};

		int				m_velocity_iterations	{20};
		int				m_position_iterations	{10};

		BroadSystem		m_broad_system;
		NarrowSystem	m_narrow_system;
		CollisionSolver	m_collision_solver;

		System<PhysicsBody>					m_integrate_velocity;
		System<PhysicsBody, BodyTransform>	m_integrate_position;
		System<PhysicsBody>					m_sleep_bodies;

		System<BodyTransform, BodyLastTransform, const Transform>	m_pre_solve;
		System<const BodyTransform, Transform>						m_post_solve;
	};
}