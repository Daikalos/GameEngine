#pragma once

#include <SFML/System/Vector2.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities/VectorUtils.h>
#include <Velox/Config.hpp>

#include "PhysicsMaterial.h"

namespace vlx
{
	enum class BodyType : unsigned char
	{
		Static,
		Dynamic,
		Kinematic
	};

	class VELOX_API PhysicsBody : public IComponent
	{
	public:
		NODISC constexpr BodyType GetType() const noexcept;
		NODISC constexpr float GetGravity() const noexcept;

		NODISC constexpr float GetDensity() const noexcept;
		NODISC constexpr float GetRestitution() const noexcept;

		NODISC constexpr float GetMass() const noexcept;
		NODISC constexpr float GetInvMass() const noexcept;

		NODISC constexpr float GetInertia() const noexcept;
		NODISC constexpr float GetInvInertia() const noexcept;

		NODISC constexpr float GetStaticFriction() const noexcept;
		NODISC constexpr float GetDynamicFriction() const noexcept;

		NODISC constexpr const sf::Vector2f& GetVelocity() const noexcept;
		NODISC constexpr const sf::Vector2f& GetForce() const noexcept;

		NODISC constexpr float GetAngularVelocity() const noexcept;
		NODISC constexpr float GetTorque() const noexcept;

	public:
		void SetType(BodyType type);
		void SetGravity(float scale);

		void SetMaterial(const PhysicsMaterial& material);

		void SetMass(float mass);
		void SetInertia(float inertia);

		void SetStaticFriction(float static_friction);
		void SetDynamicFriction(float dynamic_friction);

	public:
		void SetVelocity(const sf::Vector2f& velocity);
		void AddVelocity(const sf::Vector2f& velocity);

		void SetForce(const sf::Vector2f& force);
		void ApplyForce(const sf::Vector2f& force);

		void SetAngularVelocity(const float angular_velocity);
		void AddAngularVelocity(const float angular_velocity);

		void SetTorque(const float torque);
		void ApplyTorque(const float torque);

		void ApplyImpulse(const sf::Vector2f& impulse, const sf::Vector2f& contact_vector);

	private:
		BodyType		m_type				{BodyType::Dynamic};
		float			m_gravity_scale		{1.0f};

		PhysicsMaterial	m_material;

		float			m_mass				{1.0f};
		float			m_inv_mass			{1.0f};

		float			m_inertia			{1.0f};
		float			m_inv_inertia		{1.0f};

		float			m_static_friction	{0.5f};
		float			m_dynamic_friction	{0.3f};

		sf::Vector2f	m_velocity;
		sf::Vector2f	m_force;

		float			m_angular_velocity	{0.0f};
		float			m_torque			{0.0f};
	};
}