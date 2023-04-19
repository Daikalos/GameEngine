#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time)
	: SystemAction(entity_admin, id), 

	m_time(&time), 

	m_broad_system(		entity_admin, LYR_BROAD_PHASE),
	m_narrow_system(	entity_admin, LYR_NARROW_PHASE, m_broad_system),

	m_integrate_velocity(	entity_admin, id),
	m_integrate_position(	entity_admin, id),
	m_sleep_bodies(			entity_admin, id),
	m_post_update(			entity_admin, id),
	m_interp(				entity_admin, id)

{
	m_integrate_velocity.Each([this](EntityID entity_id, PhysicsBody& body)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() != BodyType::Dynamic)
				return;

			body.AddVelocity(m_time->GetFixedDT() * body.GetInvMass() * (body.GetGravityScale() * body.GetMass() * m_gravity + body.GetForce()));
			body.AddAngularVelocity(m_time->GetFixedDT() * body.GetInvInertia() * body.GetTorque());

			body.m_velocity *= (1.0f / (1.0f + m_time->GetFixedDT() * body.GetLinearDamping()));
			body.m_angular_velocity *= (1.0f / (1.0f + m_time->GetFixedDT() * body.GetAngularDamping()));
		});

	m_integrate_position.Each([this](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			body.last_pos = body.curr_pos;
			body.last_rot = body.curr_rot;

			local_transform.Move(body.GetVelocity() * m_time->GetFixedDT());
			local_transform.Rotate(sf::radians(body.GetAngularVelocity() * m_time->GetFixedDT()));

			body.m_force = Vector2f::Zero;
			body.m_torque = 0.0f;
		});

	m_sleep_bodies.Each([this](EntityID entity_id, PhysicsBody& body)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			// TODO: fix velocity always being above threshold even when object is still, e.g., 
			// body "resting" ontop another object, however due to continually colliding and position correction,
			// body will keep on jittering.

			constexpr float vel_sleep_tolerance = au::Sqr(0.01f);
			constexpr float ang_sleep_tolerance = au::Sqr(au::ToRadians(2.0f));

			if ((body.m_flags & PhysicsBody::B_AutoSleep) == 0 ||
				au::Sqr(body.GetAngularVelocity()) > ang_sleep_tolerance ||
				body.GetVelocity().LengthSq() > vel_sleep_tolerance)
			{
				body.m_sleep_time = 0.0f;
			}
			else
			{
				body.m_sleep_time += m_time->GetRealDT();
			}

			if (body.m_sleep_time >= 0.5f)
				body.SetAwake(false);
		});

	m_post_update.Each([&time](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			body.curr_pos = local_transform.GetPosition();
			body.curr_rot = local_transform.GetRotation();
		});

	m_interp.Each([&time](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			if (body.initialize)
			{
				body.last_pos = local_transform.GetPosition();
				body.last_rot = local_transform.GetRotation();

				body.curr_pos = body.last_pos;
				body.curr_rot = body.last_rot;

				body.initialize = false;
			}

			local_transform.SetPosition(Vector2f::Lerp(
				body.last_pos, body.curr_pos, time.GetAlpha()));

			local_transform.SetRotation(au::Lerp(
				body.last_rot, body.curr_rot, time.GetAlpha()));
		});
}

bool PhysicsSystem::IsRequired() const noexcept
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

void PhysicsSystem::SetIterations(int iterations)
{
	m_iterations = iterations;
}

void PhysicsSystem::PreUpdate()
{

}

void PhysicsSystem::Update()
{

}

void PhysicsSystem::FixedUpdate()
{
	m_broad_system.Update();

	m_narrow_system.Update(
		m_broad_system.GetPairs(), 
		m_broad_system.GetIndices());

	auto arbiters = m_narrow_system.GetArbiters();

	m_integrate_velocity.ForceRun();

	for (auto& collision : arbiters)
		Initialize(collision);

	for (int i = 0; i < m_iterations; ++i)
		for (auto& collision : arbiters)
			ResolveCollision(collision);

	m_integrate_position.ForceRun();

	for (auto& collision : arbiters)
		PositionalCorrection(collision);

	m_post_update.ForceRun();

	m_sleep_bodies.ForceRun();
}

void PhysicsSystem::PostUpdate()
{
	m_interp.ForceRun();
}

void PhysicsSystem::Initialize(CollisionArbiter& arbiter)
{
	PhysicsBody& AB = *arbiter.A->body;
	PhysicsBody& BB = *arbiter.B->body;

	arbiter.restitution = std::min(AB.GetRestitution(), BB.GetRestitution());

	arbiter.static_friction = std::sqrt(AB.GetStaticFriction() * BB.GetStaticFriction());
	arbiter.dynamic_friction = std::sqrt(AB.GetDynamicFriction() * BB.GetDynamicFriction());

	for (uint32 i = 0; i < arbiter.contacts_count; ++i)
	{
		CollisionContact& contact = arbiter.contacts[i];

		contact.ra = contact.position - arbiter.A->shape->GetCenter();
		contact.rb = contact.position - arbiter.B->shape->GetCenter();

		{
			float rna = contact.ra.Cross(contact.normal);
			float rnb = contact.rb.Cross(contact.normal);

			float k_normal = AB.GetInvMass() + BB.GetInvMass() +
				AB.GetInvInertia() * au::Sqr(rna) + BB.GetInvInertia() * au::Sqr(rnb);

			contact.mass_normal = 1.0f / k_normal;
		}

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

		{
			Vector2f tangent = rv - (contact.normal * rv.Dot(contact.normal));
			tangent = tangent.Normalize();

			float rta = contact.ra.Cross(tangent);
			float rtb = contact.rb.Cross(tangent);

			float k_tangent = AB.GetInvMass() + BB.GetInvMass() +
				AB.GetInvInertia() * au::Sqr(rta) + BB.GetInvInertia() * au::Sqr(rtb);

			contact.mass_tangent = 1.0f / k_tangent;
		}

		if (rv.LengthSq() < (m_gravity * m_time->GetFixedDT()).LengthSq() + FLT_EPSILON)
			arbiter.restitution = 0.0f;
	}
}

void PhysicsSystem::ResolveCollision(CollisionArbiter& arbiter)
{
	CollisionObject& A = *arbiter.A;
	CollisionObject& B = *arbiter.B;
	PhysicsBody& AB = *A.body;
	PhysicsBody& BB = *B.body;

	if (au::Equal(AB.GetInvMass() + BB.GetInvMass(), 0.0f))
	{
		AB.SetVelocity({});
		BB.SetVelocity({});

		return;
	}

	const Shape& AS = *A.shape;
	const Shape& BS = *B.shape;

	for (std::size_t i = 0; i < arbiter.contacts_count; ++i)
	{
		CollisionContact& contact = arbiter.contacts[i];

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

		const float vel_along_normal = rv.Dot(contact.normal);

		if (vel_along_normal >= 0.0f) // no need to resolve if they are separating
			return;

		float dpn = (1.0f + arbiter.restitution) * -vel_along_normal * contact.mass_normal;
		dpn = std::max(dpn, 0.0f);

		Vector2f impulse = dpn * contact.normal;

		AB.ApplyImpulse(-impulse, contact.ra);
		BB.ApplyImpulse( impulse, contact.rb);

		// friction

		rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
			 AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

		Vector2f tangent = rv - (contact.normal * rv.Dot(contact.normal));
		tangent = tangent.Normalize();

		float dpt = -rv.Dot(tangent) * contact.mass_tangent;

		if (au::Equal(dpt, 0.0f, P_EPSILON))
			return;

		const Vector2f friction_impulse = (std::abs(dpt) < dpn * arbiter.static_friction) ?
			(dpt * tangent) : (-dpn * tangent * arbiter.dynamic_friction); // true is static friction, false is dynamic friction

		AB.ApplyImpulse(-friction_impulse, contact.ra);
		BB.ApplyImpulse( friction_impulse, contact.rb);
	}
}

void PhysicsSystem::PositionalCorrection(CollisionArbiter& arbiter)
{
	PhysicsBody& AB = *arbiter.A->body;
	PhysicsBody& BB = *arbiter.B->body;

	constexpr float percent = 0.40f;
	constexpr float k_slop	= 0.05f;

	for (std::size_t i = 0; i < arbiter.contacts_count; ++i)
	{
		CollisionContact& contact = arbiter.contacts[i];

		Vector2f correction = (std::max(contact.penetration - k_slop, 0.0f) /
			(AB.GetInvMass() + BB.GetInvMass())) * contact.normal * percent;

		arbiter.A->local_transform->Move(-correction * AB.GetInvMass());
		arbiter.B->local_transform->Move( correction * BB.GetInvMass());
	}
}