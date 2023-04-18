#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/System/Vector2.hpp>
#include <Velox/Utility/ArithmeticUtils.h>
#include <Velox/Config.hpp>

#include "PhysicsMaterial.h"

namespace vlx
{
	class VELOX_API PhysicsBody : public IComponent
	{
	public:
		enum class Type : unsigned char
		{
			Static,		// unnable to move and cannot be affected by any external force
			Dynamic,	// affected by forces and interacts with all types
			Kinematic	// cannot be affected by external forces but may move
		};

	private:
		enum
		{
			B_Awake			= 1 << 0,
			B_AutoSleep		= 1 << 1,
			B_FixedRotation = 1 << 2,
			B_Enabled		= 1 << 3
		};

	public:
		NODISC constexpr auto GetType() const noexcept -> Type;

		NODISC constexpr const Vector2f& GetVelocity() const noexcept;
		NODISC constexpr const Vector2f& GetForce() const noexcept;

		NODISC constexpr float GetAngularVelocity() const noexcept;
		NODISC constexpr float GetTorque() const noexcept;

		NODISC constexpr float GetDensity() const noexcept;
		NODISC constexpr float GetRestitution() const noexcept;

		NODISC constexpr float GetMass() const noexcept;
		NODISC constexpr float GetInvMass() const noexcept;

		NODISC constexpr float GetInertia() const noexcept;
		NODISC constexpr float GetInvInertia() const noexcept;

		NODISC constexpr float GetStaticFriction() const noexcept;
		NODISC constexpr float GetDynamicFriction() const noexcept;

		NODISC constexpr float GetLinearDamping() const noexcept;
		NODISC constexpr float GetAngularDamping() const noexcept;

		NODISC constexpr float GetGravityScale() const noexcept;

		NODISC constexpr bool IsAwake() const noexcept;
		NODISC constexpr bool IsEnabled() const noexcept;
		NODISC constexpr bool IsFixedRotation() const noexcept;
		NODISC constexpr bool IsSleepingAllowed() const noexcept;

	public:
		constexpr void SetType(const Type type);

		constexpr void SetVelocity(const Vector2f& velocity);
		constexpr void AddVelocity(const Vector2f& velocity);

		constexpr void SetAngularVelocity(const float angular_velocity);
		constexpr void AddAngularVelocity(const float angular_velocity);

		constexpr void AddForce(const Vector2f& force, const bool wake = true);
		constexpr void AddTorque(const float torque, const bool wake = true);

		constexpr void ApplyForce(const Vector2f& force, const Vector2f& point, const bool wake = true);

		constexpr void ApplyImpulseToCenter(const Vector2f& impulse, const bool wake = true);
		constexpr void ApplyAngularImpulse(const float impulse, const  bool wake = true);

		constexpr void ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector, const bool wake = true);

		constexpr void SetMaterial(const PhysicsMaterial& material);

		constexpr void SetMass(const float mass);
		constexpr void SetInertia(const float inertia);

		constexpr void SetStaticFriction(const float static_friction);
		constexpr void SetDynamicFriction(const float dynamic_friction);

		constexpr void SetLinearDamping(const float linear_damping);
		constexpr void SetAngularDamping(const float angular_damping);

		constexpr void SetGravityScale(const float scale);

		constexpr void SetAwake(const bool flag);
		constexpr void SetSleepingAllowed(const bool flag);
		constexpr void SetFixedRotation(const bool flag);
		constexpr void SetEnabled(const bool flag);

	private:
		Vector2f last_pos;
		Vector2f curr_pos;

		sf::Angle last_rot;
		sf::Angle curr_rot;

		bool initialize {true};

	private:
		Type			m_type				{Type::Dynamic}; // type of body

		Vector2f		m_velocity;					// velocity of body
		float			m_angular_velocity	{0.0f}; // angular velocity of body

		Vector2f		m_force;					// force being applied on body
		float			m_torque			{0.0f};	// torque being applied on body

		PhysicsMaterial	m_material			{1.0f, 0.2f}; // the material this body is made of

		float			m_mass				{0.0f};	// mass of body
		float			m_inv_mass			{0.0f};	// inverse mass

		float			m_inertia			{0.0f}; // inertia of body
		float			m_inv_inertia		{0.0f};	// inverse inertia

		float			m_static_friction	{0.5f}; // static friction of body
		float			m_dynamic_friction	{0.3f}; // dynamic friction of body

		float			m_linear_damping	{0.0f};
		float			m_angular_damping	{0.0f};

		float			m_gravity_scale		{1.0f};
		float			m_sleep_time		{0.0f};
		
		uint16_t		m_flags				{B_Enabled | B_Awake};

		friend class PhysicsSystem;
	};

	constexpr auto	PhysicsBody::GetType() const noexcept -> Type			{ return m_type; }

	constexpr const Vector2f& PhysicsBody::GetVelocity() const noexcept		{ return m_velocity; }
	constexpr const Vector2f& PhysicsBody::GetForce() const noexcept		{ return m_force; }
	constexpr float PhysicsBody::GetAngularVelocity() const noexcept		{ return m_angular_velocity; }
	constexpr float PhysicsBody::GetTorque() const noexcept					{ return m_torque; }

	constexpr float PhysicsBody::GetDensity() const noexcept				{ return m_material.density; }
	constexpr float PhysicsBody::GetRestitution() const noexcept			{ return m_material.restitution; }

	constexpr float PhysicsBody::GetMass() const noexcept					{ return m_mass; }
	constexpr float PhysicsBody::GetInvMass() const noexcept				{ return m_inv_mass; }
	constexpr float PhysicsBody::GetInertia() const noexcept				{ return m_inertia; }
	constexpr float PhysicsBody::GetInvInertia() const noexcept				{ return m_inv_inertia; }

	constexpr float PhysicsBody::GetStaticFriction() const noexcept			{ return m_static_friction; }
	constexpr float PhysicsBody::GetDynamicFriction() const noexcept		{ return m_dynamic_friction; }

	constexpr float PhysicsBody::GetLinearDamping() const noexcept			{ return m_linear_damping; }
	constexpr float PhysicsBody::GetAngularDamping() const noexcept			{ return m_angular_damping; }

	constexpr float PhysicsBody::GetGravityScale() const noexcept			{ return m_gravity_scale; }

	constexpr bool PhysicsBody::IsAwake() const noexcept					{ return (m_flags & B_Awake) == B_Awake; }
	constexpr bool PhysicsBody::IsEnabled() const noexcept					{ return (m_flags & B_Enabled) == B_Enabled; }
	constexpr bool PhysicsBody::IsFixedRotation() const noexcept			{ return (m_flags & B_FixedRotation) == B_FixedRotation; }
	constexpr bool PhysicsBody::IsSleepingAllowed() const noexcept			{ return (m_flags & B_AutoSleep) == B_AutoSleep; }

	constexpr void PhysicsBody::SetType(const Type type)
	{
		if (m_type == type)
			return;

		m_type = type;

		if (m_type == Type::Static)
		{
			m_velocity = Vector2f::Zero;
			m_angular_velocity = 0.0f;
			m_flags &= ~B_Awake;
		}

		SetAwake(true);

		m_force = Vector2f::Zero;
		m_torque = 0.0f;
	}

	constexpr void PhysicsBody::SetVelocity(const Vector2f& velocity)
	{
		if (m_type == Type::Static)
			return;

		if (m_velocity == velocity)
			return;

		if (velocity.LengthSq() > au::Sqr(PHYSICS_EPSILON))
			SetAwake(true);

		m_velocity = velocity;
	}
	constexpr void PhysicsBody::AddVelocity(const Vector2f& velocity)
	{
		SetVelocity(GetVelocity() + velocity);
	}

	constexpr void PhysicsBody::SetAngularVelocity(const float angular_velocity)
	{
		if (m_type == Type::Static)
			return;

		if (m_angular_velocity == angular_velocity)
			return;

		if (angular_velocity * angular_velocity > PHYSICS_EPSILON)
			SetAwake(true);

		if (m_flags & B_Awake)
			m_angular_velocity = angular_velocity;
	}
	constexpr void PhysicsBody::AddAngularVelocity(const float angular_velocity)
	{
		SetAngularVelocity(GetAngularVelocity() + angular_velocity);
	}

	constexpr void PhysicsBody::AddForce(const Vector2f& force, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
			m_force += force;
	}
	constexpr void PhysicsBody::AddTorque(const float torque, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
			m_torque += torque;
	}

	inline constexpr void PhysicsBody::ApplyForce(const Vector2f& force, const Vector2f& point, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
		{
			m_force += force;
			m_torque += point.Cross(force);
		}
	}

	constexpr void PhysicsBody::ApplyImpulseToCenter(const Vector2f& impulse, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
			m_velocity += impulse * GetInvMass();
	}

	constexpr void PhysicsBody::ApplyAngularImpulse(const float impulse, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
			m_angular_velocity += impulse * GetInvMass();
	}

	constexpr void PhysicsBody::ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector, const bool wake)
	{
		if (m_type != Type::Dynamic)
			return;

		if (wake && (m_flags & B_Awake) == 0)
			SetAwake(true);

		if (m_flags & B_Awake)
		{
			m_velocity += impulse * GetInvMass();
			m_angular_velocity += contact_vector.Cross(impulse) * GetInvInertia();
		}
	}

	constexpr void PhysicsBody::SetMaterial(const PhysicsMaterial& material)
	{
		m_material = material;
	}

	constexpr void PhysicsBody::SetMass(const float mass)
	{
		if (m_type != Type::Dynamic)
			return;

		m_mass = mass;

		if (m_mass <= 0.0f)
			m_mass = 1.0f;

		m_inv_mass = (1.0f / m_mass);
	}

	constexpr void PhysicsBody::SetInertia(const float inertia)
	{
		if (m_type != Type::Dynamic)
			return;

		m_inertia = 0.0f;
		m_inv_inertia = 0.0f;

		if ((m_flags & B_FixedRotation) == 0)
		{
			m_inertia = inertia;
			m_inv_inertia = (m_inertia != 0.0f) ? (1.0f / m_inertia) : 0.0f;
		}
	}

	constexpr void PhysicsBody::SetStaticFriction(const float static_friction)
	{
		m_static_friction = static_friction;
	}

	constexpr void PhysicsBody::SetDynamicFriction(const float dynamic_friction)
	{
		m_dynamic_friction = dynamic_friction;
	}

	constexpr void PhysicsBody::SetLinearDamping(const float linear_damping)
	{
		m_linear_damping = linear_damping;
	}

	constexpr void PhysicsBody::SetAngularDamping(const float angular_damping)
	{
		m_angular_damping = angular_damping;
	}

	constexpr void PhysicsBody::SetGravityScale(const float scale)
	{
		m_gravity_scale = scale;
	}

	constexpr void PhysicsBody::SetAwake(const bool flag)
	{
		if (m_type == Type::Static)
			return;

		if (flag)
		{
			m_flags |= B_Awake;
			m_sleep_time = 0.0f;
		}
		else
		{
			m_flags &= ~B_Awake;
			m_sleep_time = 0.0f;
			m_velocity = Vector2f::Zero;
			m_angular_velocity = 0.0f;
			m_force = Vector2f::Zero;
			m_torque = 0.0f;
		}
	}

	constexpr void PhysicsBody::SetSleepingAllowed(const bool flag)
	{
		if (flag)
		{
			m_flags |= B_AutoSleep;
		}
		else
		{
			m_flags &= ~B_AutoSleep;
			SetAwake(true);
		}
	}

	constexpr void PhysicsBody::SetFixedRotation(const bool flag)
	{
		bool status = (m_flags & B_FixedRotation) == B_FixedRotation;

		if (status == flag)
			return;

		if (flag)
		{
			m_flags |= B_FixedRotation;
			SetInertia(0.0f);
		}
		else
		{
			m_flags &= ~B_FixedRotation;
		}

		m_angular_velocity = 0.0f;
	}

	constexpr void PhysicsBody::SetEnabled(const bool flag)
	{
		if (flag)
		{
			m_flags |= B_Enabled;
		}
		else
		{
			m_flags &= ~B_Enabled;
			// todo: tell broad phase to remove from quad tree
		}
	}
}