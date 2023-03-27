#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time, NarrowSystem& narrow_system)
	: SystemAction(entity_admin, id), 

	m_time(&time), 
	m_narrow_system(&narrow_system),

	m_update_forces(	entity_admin, LYR_PHYSICS + 1),
	m_update_positions(	entity_admin, LYR_PHYSICS + 2),
	m_clear_forces(		entity_admin, LYR_PHYSICS + 3),
	m_sleep_bodies(		entity_admin, LYR_PHYSICS + 4)

{
	m_update_forces.Each([this](EntityID entity_id, PhysicsBody& body)
		{
			if (body.GetInvMass() <= PHYSICS_EPSILON)
				return;

			body.AddVelocity((body.GetForce() * body.GetInvMass() + m_gravity) * (m_time->GetFixedDT() / 2.0f));
			body.AddAngularVelocity(body.GetTorque() * body.GetInvInertia() * (m_time->GetFixedDT() / 2.0f));
		});

	m_update_positions.Each([this](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			if (body.GetInvMass() <= PHYSICS_EPSILON)
				return;

			local_transform.Move(body.GetVelocity() * m_time->GetFixedDT());
			local_transform.Rotate(sf::radians(body.GetAngularVelocity() * m_time->GetFixedDT()));

			body.AddVelocity((body.GetForce() * body.GetInvMass() + m_gravity) * (m_time->GetFixedDT() / 2.0f));
			body.AddAngularVelocity(body.GetTorque() * body.GetInvInertia() * (m_time->GetFixedDT() / 2.0f));
		});

	m_clear_forces.Each([](EntityID entity_id, PhysicsBody& body)
		{
			body.SetForce({});
			body.SetTorque(0.0f);
		});

	m_sleep_bodies.Each([this](EntityID entity_id, PhysicsBody& body)
		{
			// TODO: check if body moved this frame, if not, decrement timer, otherwise, set timer to max

			if (body.m_awake)
			{
				body.m_sleep_timer -= m_time->GetRealDT();
				if (body.m_sleep_timer <= 0.0f)
					body.m_awake = false;
			}
		});
}

constexpr bool PhysicsSystem::IsRequired() const noexcept
{
	return true;
}

const Vector2f& PhysicsSystem::GetGravity() const
{
	return m_gravity;
}

void PhysicsSystem::SetGravity(const Vector2f& gravity)
{
	m_gravity = gravity;
}

void PhysicsSystem::PreUpdate()
{

}

void PhysicsSystem::Update()
{

}

void PhysicsSystem::FixedUpdate()
{
	std::span<CollisionData> collisions_data = m_narrow_system->m_collision_data;

	m_update_forces.ForceRun();

	for (auto& collision : collisions_data)
		Initialize(collision);

	for (int i = 0; i < 10; ++i)
		for (auto& collision : collisions_data)
			ResolveCollision(collision);

	m_update_positions.ForceRun();

	for (auto& collision : collisions_data)
		PositionalCorrection(collision);

	m_clear_forces.ForceRun();

	m_sleep_bodies.ForceRun();

	std::puts(std::to_string(collisions_data.size()).c_str());
}

void PhysicsSystem::PostUpdate()
{

}

void PhysicsSystem::Initialize(CollisionData& collision)
{
	if (collision.A->body == nullptr ||
		collision.B->body == nullptr)
		return;

	PhysicsBody& AB = *collision.A->body;
	PhysicsBody& BB = *collision.B->body;

	collision.restitution = std::min(AB.GetRestitution(), BB.GetRestitution());

	collision.static_friction = std::sqrt(AB.GetStaticFriction() * BB.GetStaticFriction());
	collision.dynamic_friction = std::sqrt(AB.GetDynamicFriction() * BB.GetDynamicFriction());

	for (std::uint32_t i = 0; i < collision.contact_count; ++i)
	{
		Vector2f ra = collision.contacts[i] - collision.A->shape->GetCenter();
		Vector2f rb = collision.contacts[i] - collision.B->shape->GetCenter();

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		if (rv.LengthSq() < (m_gravity * m_time->GetFixedDT()).LengthSq() + FLT_EPSILON)
			collision.restitution = 0.0f;
	}
}

void PhysicsSystem::ResolveCollision(CollisionData& collision)
{
	if (collision.A->body == nullptr ||
		collision.B->body == nullptr)
		return;

	PhysicsBody& AB = *collision.A->body;
	PhysicsBody& BB = *collision.B->body;

	if (au::Equal(AB.GetInvMass() + BB.GetInvMass(), 0.0f))
	{
		AB.SetVelocity({});
		BB.SetVelocity({});

		return;
	}

	for (std::size_t i = 0; i < collision.contact_count; ++i)
	{
		Vector2f ra = collision.contacts[i] - collision.A->shape->GetCenter();
		Vector2f rb = collision.contacts[i] - collision.B->shape->GetCenter();

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		const float vel_along_normal = rv.Dot(collision.normal);

		if (vel_along_normal > 0.0f) // no need to resolve if they are separating
			return;

		const float ra_cross_n = ra.Cross(collision.normal);
		const float rb_cross_n = rb.Cross(collision.normal);

		const float inv_mass_sum = AB.GetInvMass() + BB.GetInvMass() +
			au::Sq(ra_cross_n) * AB.GetInvInertia() + au::Sq(rb_cross_n) * BB.GetInvInertia();

		// impulse scalar
		float j = -(1.0f + collision.restitution) * vel_along_normal;
		j /= inv_mass_sum;
		j /= (float)collision.contact_count;

		const Vector2f impulse = collision.normal * j;
		AB.ApplyImpulse(-impulse, ra);
		BB.ApplyImpulse( impulse, rb);



		rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
			 AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		Vector2f t = rv - (collision.normal * rv.Dot(collision.normal));

		float length_sqr = t.LengthSq();
		if (length_sqr <= FLT_EPSILON)
			return;

		t = t.Normalize(std::sqrt(length_sqr), 1.0f);

		float jt = -rv.Dot(t);
		jt /= inv_mass_sum;
		jt /= (float)collision.contact_count;

		if (au::Equal(jt, 0.0f))
			return;

		const Vector2f tangent_impulse = (std::abs(jt) < j * collision.static_friction) ? t * jt : t * -j * collision.dynamic_friction;

		AB.ApplyImpulse(-tangent_impulse, ra);
		BB.ApplyImpulse( tangent_impulse, rb);
	}
}

void PhysicsSystem::PositionalCorrection(CollisionData& collision)
{
	if (collision.A->body == nullptr ||
		collision.B->body == nullptr)
		return;

	PhysicsBody& AB = *collision.A->body;
	PhysicsBody& BB = *collision.B->body;

	const float percent = 0.20f;
	const float k_slop	= 0.01f;

	Vector2f correction = (std::max(collision.penetration - k_slop, 0.0f) / 
		(AB.GetInvMass() + BB.GetInvMass())) * collision.normal * percent;

	collision.A->local_transform->Move(-correction * AB.GetInvMass());
	collision.B->local_transform->Move( correction * BB.GetInvMass());
}