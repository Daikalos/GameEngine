#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, LayerType id, Time& time)
	: SystemAction(entity_admin, id, true), 

	m_time(&time), 

	m_broad_system(			entity_admin, LYR_BROAD_PHASE),
	m_narrow_system(		entity_admin, LYR_NARROW_PHASE),

	m_integrate_velocity(	entity_admin),
	m_integrate_position(	entity_admin),
	m_sleep_bodies(			entity_admin),
	m_pre_solve(			entity_admin)

{
	m_integrate_velocity.Each(
		[this](EntityID entity_id, PhysicsBody& pb)
		{
			if (!pb.IsAwake() || !pb.IsEnabled())
				return;

			if (pb.GetType() != BodyType::Dynamic)
				return;

			// velocity
			pb.m_velocity			+= m_time->GetFixedDT() * pb.GetInvMass() * (m_gravity * pb.GetGravityScale() * pb.GetMass() + pb.GetForce());
			pb.m_angular_velocity	+= m_time->GetFixedDT() * pb.GetInvInertia() * pb.GetTorque();

			// damping
			pb.m_velocity			*= (1.0f / (1.0f + m_time->GetFixedDT() * pb.GetLinearDamping()));
			pb.m_angular_velocity	*= (1.0f / (1.0f + m_time->GetFixedDT() * pb.GetAngularDamping()));
		});

	m_integrate_position.Each(
		[this](EntityID entity_id, PhysicsBody& pb, Transform& t)
		{
			if (!pb.IsAwake() || !pb.IsEnabled())
				return;

			if (pb.GetType() == BodyType::Static)
				return;

			t.Move(pb.GetVelocity() * m_time->GetFixedDT());
			t.Rotate(sf::radians(pb.GetAngularVelocity() * m_time->GetFixedDT()));

			pb.m_force = Vector2f::Zero;
			pb.m_torque = 0.0f;
		});

	m_sleep_bodies.Each(
		[this](EntityID entity_id, PhysicsBody& body)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			constexpr float vel_sleep_tolerance = au::Sqr(P_VEL_SLEEP_TOLERANCE);
			constexpr float ang_sleep_tolerance = au::Sqr(P_ANG_SLEEP_TOLERANCE);

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

	m_pre_solve.Each(
		[this](EntityID entity_id, PhysicsBodyTransform& pbt, Transform& transform)
		{
			pbt.m_last_pos = transform.GetPosition(); // possible because a physics body is not allowed to have a parent
			pbt.m_last_rot = transform.GetRotation();
		});
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

void PhysicsSystem::FixedUpdate()
{
	m_broad_system.Update();
	m_narrow_system.Update(m_broad_system);

	auto& arbiters = m_narrow_system.GetArbiters();

	m_pre_solve.ForceRun();

	m_integrate_velocity.ForceRun();

	for (auto& arb : arbiters)
		arb.Initialize(*m_time, m_gravity);

	for (int i = 0; i < m_iterations; ++i)
		for (auto& arb : arbiters)
			arb.ResolveVelocity();

	m_integrate_position.ForceRun();

	for (auto& arb : arbiters)
		arb.ResolvePosition();

	m_sleep_bodies.ForceRun();
}