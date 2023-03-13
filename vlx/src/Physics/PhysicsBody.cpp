#include <Velox/Physics/PhysicsBody.h>

using namespace vlx;

constexpr BodyType PhysicsBody::GetType() const noexcept				{ return m_type; }
constexpr float PhysicsBody::GetGravity() const noexcept				{ return m_gravity_scale; }
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

void PhysicsBody::SetType(BodyType type)
{
	m_type = type;
}

void PhysicsBody::SetGravity(float scale)
{
	m_gravity_scale = scale;
}

void PhysicsBody::SetMaterial(const PhysicsMaterial& material)
{
	m_material = material;
}

void PhysicsBody::SetMass(float mass)
{
	m_mass = mass;
	m_inv_mass = (m_mass != 0.0f) ? (1.0f / m_mass) : 0.0f;
}

void PhysicsBody::SetInertia(float inertia)
{
	m_inertia = inertia;
	m_inv_inertia = (m_inertia != 0.0f) ? (1.0f / m_inertia) : 0.0f;
}

void PhysicsBody::SetStaticFriction(float static_friction)
{
	m_static_friction = static_friction;
}

void PhysicsBody::SetDynamicFriction(float dynamic_friction)
{
	m_dynamic_friction = dynamic_friction;
}

void PhysicsBody::SetVelocity(const Vector2f& velocity)
{
	if (m_velocity != velocity)
	{
		if (velocity.LengthSq() > au::Sq(PHYSICS_EPSILON))
		{
			m_velocity = velocity;
			m_awake = true;
		}
		else
		{
			m_velocity = Vector2f::Zero;
			m_awake = false;
		}
	}
}
void PhysicsBody::AddVelocity(const Vector2f& velocity)
{
	SetVelocity(GetVelocity() + velocity);
}

void PhysicsBody::SetForce(const Vector2f& force)
{
	m_force = force;
}
void PhysicsBody::AddForce(const Vector2f& force)
{
	SetForce(GetForce() + force);
}

void PhysicsBody::SetAngularVelocity(const float angular_velocity)
{
	m_angular_velocity = angular_velocity;
}
void PhysicsBody::AddAngularVelocity(const float angular_velocity)
{
	SetAngularVelocity(GetAngularVelocity() + angular_velocity);
}

void PhysicsBody::SetTorque(const float torque)
{
	m_torque = torque;
}
void PhysicsBody::AddTorque(const float torque)
{
	SetTorque(GetTorque() + torque);
}

void PhysicsBody::ApplyImpulse(const Vector2f& impulse, const Vector2f& contact_vector)
{
	AddVelocity(impulse * GetInvMass());
	AddAngularVelocity(contact_vector.Cross(impulse) * GetInvInertia());
}
