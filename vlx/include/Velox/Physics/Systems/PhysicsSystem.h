#pragma once

#include <vector>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/LocalTransform.h>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>
#include <Velox/Utilities.hpp>




namespace vlx
{
	class PhysicsSystem final : public SystemAction
	{
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

	private:
		//void Initialize(CollisionData& data);
		//void ResolveCollision(CollisionData& data);
		//void PositionalCorrection(CollisionData& data);

	private:
		Time*			m_time		{nullptr};
		sf::Vector2f	m_gravity	{0.0f, 10.0f};

		//System<PhysicsBody>					m_update_forces;
		//System<PhysicsBody, LocalTransform> m_update_velocities;
		//System<PhysicsBody>					m_clear_forces;
	};
}