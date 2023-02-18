#pragma once

#include <vector>

#include <Velox/Components/Transform.h>
#include <Velox/Components/LocalTransform.h>

#include <Velox/World/SystemObject.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Utilities.hpp>

#include "Shape.h"
#include "Circle.h"
#include "CollisionResult.h"
#include "CollisionData.h"

namespace vlx
{
	class PhysicsSystem : public SystemObject
	{
	private:
		using CircleSystem = System<Circle, LocalTransform, Transform>;

		struct CollisionPair
		{
			Shape* A;
			Shape* B;
		};

	public:
		PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time);

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void UpdateForces(PhysicsBody* body);
		void UpdateVelocity(PhysicsBody* body, LocalTransform& local_transform);

	private:
		void Initialize(CollisionData& data);
		void ResolveCollision(CollisionData& data);
		void PositionalCorrection(CollisionData& data);

	private:
		Time*			m_time		{nullptr};
		sf::Vector2f	m_gravity	{0.0f, 10.0f};

		//System m_
		//System m_clear_forces;
		//System m_update_velocities;

		std::vector<CollisionPair>	m_collision_pairs;
		std::vector<CollisionData>	m_collision_results;
	};
}