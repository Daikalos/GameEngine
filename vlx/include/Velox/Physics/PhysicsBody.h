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
		void SetType(BodyType type);
		void SetGravity(float scale);

		void SetMaterial(const PhysicsMaterial& material);

		void SetMass(float mass);
		void SetInertia(float inertia);

		void SetStaticFriction(float static_friction);
		void SetDynamicFriction(float dynamic_friction);

		void SetVelocity(const Vector2f& velocity);
		void AddVelocity(const Vector2f& velocity);

		void SetForce(const Vector2f& force);
		void AddForce(const Vector2f& force);

		void SetAngularVelocity(const float angular_velocity);
		void AddAngularVelocity(const float angular_velocity);

		void SetTorque(const float torque);
		void AddTorque(const float torque);

		void ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector);

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
}