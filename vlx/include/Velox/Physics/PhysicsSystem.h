#pragma once

#include <vector>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>

#include <Velox/ECS/SystemObject.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Utilities.hpp>

#include "Shape.h"
#include "Circle.h"
#include "CollisionResult.h"
#include "CollisionData.h"
#include "CollisionTable.h"

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

	public:


	private:
		void Initialize(CollisionData& data);
		void ResolveCollision(CollisionData& data);
		void PositionalCorrection(CollisionData& data);

	private:
		Time*			m_time		{nullptr};
		sf::Vector2f	m_gravity	{0.0f, 10.0f};

		System<PhysicsBody>					m_update_forces;
		System<PhysicsBody, LocalTransform> m_update_velocities;
		System<PhysicsBody>					m_clear_forces;

		//System m_
		//System m_clear_forces;
		//System m_update_velocities;

		std::vector<CollisionPair>	m_collisions_pair;
		std::vector<CollisionData>	m_collisions_data;
	};
}