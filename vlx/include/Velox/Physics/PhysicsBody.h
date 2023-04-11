#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/System/Vector2.hpp>
#include <Velox/Utility/ArithmeticUtils.h>
#include <Velox/Config.hpp>

#include "PhysicsMaterial.h"

namespace vlx
{
	enum class BodyType : unsigned char
	{
		Static,		// unnable to move and cannot be affected by any external force
		Dynamic,	// affected by forces and interacts with all types
		Kinematic	// cannot be affected by external forces but may move
	};

	class VELOX_API PhysicsBody : public IComponent
	{
	public:
		NODISC constexpr BodyType GetType() const noexcept;
		NODISC constexpr float GetGravityScale() const noexcept;

		NODISC constexpr float GetDensity() const noexcept;
		NODISC constexpr float GetRestitution() const noexcept;

		NODISC constexpr float GetMass() const noexcept;
		NODISC constexpr float GetInvMass() const noexcept;

		NODISC constexpr float GetInertia() const noexcept;
		NODISC constexpr float GetInvInertia() const noexcept;

		NODISC constexpr float GetStaticFriction() const noexcept;
		NODISC constexpr float GetDynamicFriction() const noexcept;

		NODISC constexpr const Vector2f& GetVelocity() const noexcept;
		NODISC constexpr const Vector2f& GetForce() const noexcept;

		NODISC constexpr float GetAngularVelocity() const noexcept;
		NODISC constexpr float GetTorque() const noexcept;

	public:
		constexpr void SetType(BodyType type);
		constexpr void SetGravity(float scale);

		constexpr void SetMaterial(const PhysicsMaterial& material);

		constexpr void SetMass(float mass);
		constexpr void SetInertia(float inertia);

		constexpr void SetStaticFriction(float static_friction);
		constexpr void SetDynamicFriction(float dynamic_friction);

		constexpr void SetVelocity(const Vector2f& velocity);
		constexpr void AddVelocity(const Vector2f& velocity);

		constexpr void SetForce(const Vector2f& force);
		constexpr void AddForce(const Vector2f& force);

		constexpr void SetAngularVelocity(const float angular_velocity);
		constexpr void AddAngularVelocity(const float angular_velocity);

		constexpr void SetTorque(const float torque);
		constexpr void AddTorque(const float torque);

		constexpr void ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector);

	private:
		BodyType		m_type				{BodyType::Dynamic}; // type of body
		float			m_gravity_scale		{1.0f};

		PhysicsMaterial	m_material			{1.0f, 0.2f}; // the material this body is made of

		Vector2f		m_velocity;					// velocity of body
		float			m_angular_velocity	{0.0f}; // angular velocity of body

		Vector2f		m_force;					// force being applied on body
		float			m_torque			{0.0f};	// torque being applied on body

		float			m_mass				{0.0f};	// mass of body
		float			m_inv_mass			{0.0f};	// inverse mass

		float			m_inertia			{0.0f}; // inertia of body
		float			m_inv_inertia		{0.0f};	// inverse inertia

		float			m_static_friction	{0.5f}; // static friction of body
		float			m_dynamic_friction	{0.3f}; // dynamic friction of body

		float			m_sleep_timer_max	{2.0f};
		float			m_sleep_timer		{0.0f};
		float			m_sensitivity		{10.0f};
		bool			m_awake				{true};

		friend class PhysicsSystem;
	};

	constexpr BodyType PhysicsBody::GetType() const noexcept				{ return m_type; }
	constexpr float PhysicsBody::GetGravityScale() const noexcept			{ return m_gravity_scale; }
	constexpr float PhysicsBody::GetDensity() const noexcept				{ return m_material.density; }
	constexpr float PhysicsBody::GetRestitution() const noexcept			{ return m_material.restitution; }
	constexpr float PhysicsBody::GetMass() const noexcept					{ return m_mass; }
	constexpr float PhysicsBody::GetInvMass() const noexcept				{ return m_inv_mass; }
	constexpr float PhysicsBody::GetInertia() const noexcept				{ return m_inertia; }
	constexpr float PhysicsBody::GetInvInertia() const noexcept				{ return m_inv_inertia; }
	constexpr float PhysicsBody::GetStaticFriction() const noexcept			{ return m_static_friction; }
	constexpr float PhysicsBody::GetDynamicFriction() const noexcept		{ return m_dynamic_friction; }
	constexpr const Vector2f& PhysicsBody::GetVelocity() const noexcept		{ return m_velocity; }
	constexpr const Vector2f& PhysicsBody::GetForce() const noexcept		{ return m_force; }
	constexpr float vlx::PhysicsBody::GetAngularVelocity() const noexcept	{ return m_angular_velocity; }
	constexpr float vlx::PhysicsBody::GetTorque() const noexcept			{ return m_torque; }

	constexpr void PhysicsBody::SetType(BodyType type)
	{
		m_type = type;
	}

	constexpr void PhysicsBody::SetGravity(float scale)
	{
		m_gravity_scale = scale;
	}

	constexpr void PhysicsBody::SetMaterial(const PhysicsMaterial& material)
	{
		m_material = material;
	}

	constexpr void PhysicsBody::SetMass(float mass)
	{
		m_mass = mass;
		m_inv_mass = (m_mass != 0.0f) ? (1.0f / m_mass) : 0.0f;
	}

	constexpr void PhysicsBody::SetInertia(float inertia)
	{
		m_inertia = inertia;
		m_inv_inertia = (m_inertia != 0.0f) ? (1.0f / m_inertia) : 0.0f;
	}

	constexpr void PhysicsBody::SetStaticFriction(float static_friction)
	{
		m_static_friction = static_friction;
	}

	constexpr void PhysicsBody::SetDynamicFriction(float dynamic_friction)
	{
		m_dynamic_friction = dynamic_friction;
	}

	constexpr void PhysicsBody::SetVelocity(const Vector2f& velocity)
	{
		if (m_velocity != velocity)
		{
			if (velocity.LengthSq() > au::Sqr(PHYSICS_EPSILON))
			{
				m_velocity = velocity;

				m_sleep_timer = m_sleep_timer_max;
				m_awake = true;
			}
			else
			{
				m_velocity = Vector2f::Zero;
			}
		}
	}
	constexpr void PhysicsBody::AddVelocity(const Vector2f& velocity)
	{
		SetVelocity(GetVelocity() + velocity);
	}

	constexpr void PhysicsBody::SetForce(const Vector2f& force)
	{
		m_force = force;
	}
	constexpr void PhysicsBody::AddForce(const Vector2f& force)
	{
		SetForce(GetForce() + force);
	}

	constexpr void PhysicsBody::SetAngularVelocity(const float angular_velocity)
	{
		if (m_angular_velocity != angular_velocity)
		{
			if (std::abs(angular_velocity) > PHYSICS_EPSILON)
			{
				m_angular_velocity = angular_velocity;

				m_sleep_timer = m_sleep_timer_max;
				m_awake = true;
			}
			else
			{
				m_angular_velocity = 0.0f;
			}
		}
	}
	constexpr void PhysicsBody::AddAngularVelocity(const float angular_velocity)
	{
		SetAngularVelocity(GetAngularVelocity() + angular_velocity);
	}

	constexpr void PhysicsBody::SetTorque(const float torque)
	{
		m_torque = torque;
	}
	constexpr void PhysicsBody::AddTorque(const float torque)
	{
		SetTorque(GetTorque() + torque);
	}

	constexpr void PhysicsBody::ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector)
	{
		AddVelocity(impulse * GetInvMass());
		AddAngularVelocity(contact_vector.Cross(impulse) * GetInvInertia());
	}
}