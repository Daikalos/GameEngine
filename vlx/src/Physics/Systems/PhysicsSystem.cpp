#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, LayerType id, Time& time)
	: SystemAction(entity_admin, id, true), 

	m_time(&time), 

	m_broad_system(			entity_admin),
	m_narrow_system(		entity_admin),

	m_integrate_velocity(	entity_admin),
	m_integrate_position(	entity_admin),
	m_sleep_bodies(			entity_admin),

	m_pre_solve(			entity_admin),
	m_post_solve(			entity_admin)

{
	m_integrate_velocity.Each(&PhysicsSystem::IntegrateVelocity, this);
	m_integrate_position.Each(&PhysicsSystem::IntegratePosition, this);
	m_sleep_bodies.Each(&PhysicsSystem::SleepBodies, this);
	m_pre_solve.Each(&PhysicsSystem::PreSolve, this);
	m_post_solve.Each(&PhysicsSystem::PostSolve, this);
}

const Vector2f& PhysicsSystem::GetGravity() const
{
	return m_gravity;
}

void PhysicsSystem::SetGravity(const Vector2f& gravity)
{
	m_gravity = gravity;
}

void PhysicsSystem::SetVelocityIterations(int iterations)
{
	m_velocity_iterations = iterations;
}

void PhysicsSystem::SetPositionIterations(int iterations)
{
	m_position_iterations = iterations;
}

void PhysicsSystem::FixedUpdate()
{
	Execute(m_pre_solve);

	m_broad_system.Update();
	m_narrow_system.Update(m_broad_system);

	const auto& bodies		= m_broad_system.GetBodies();
	const auto& collisions	= m_narrow_system.GetCollisions();
	const auto& manifolds	= m_narrow_system.GetManifolds();

	Execute(m_integrate_velocity);

	m_collision_solver.CreateConstraints(bodies, collisions, manifolds);
	m_collision_solver.SetupConstraints(bodies, collisions, manifolds, *m_time, m_gravity);

	for (int i = 0; i < m_velocity_iterations; ++i)
		m_collision_solver.ResolveVelocity(bodies, collisions);

	Execute(m_integrate_position);

	for (int i = 0; i < m_position_iterations; ++i)
	{
		if (m_collision_solver.ResolvePosition(bodies, collisions, manifolds))
			break;
	}

	Execute(m_sleep_bodies);

	Execute(m_post_solve);
}

void PhysicsSystem::IntegrateVelocity(PhysicsBody& pb) const
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
}

void PhysicsSystem::IntegratePosition(PhysicsBody& pb, BodyTransform& bt) const
{
	if (!pb.IsAwake() || !pb.IsEnabled())
		return;

	if (pb.GetType() == BodyType::Static)
		return;

	bt.m_position += pb.GetVelocity() * m_time->GetFixedDT();
	bt.m_rotation += sf::radians(pb.GetAngularVelocity() * m_time->GetFixedDT());

	pb.m_force = Vector2f::Zero;
	pb.m_torque = 0.0f;
}

void PhysicsSystem::SleepBodies(PhysicsBody& pb) const
{
	if (!pb.IsAwake() || !pb.IsEnabled())
		return;

	if (pb.GetType() == BodyType::Static)
		return;

	constexpr float vel_sleep_tolerance = au::Sqr(P_VEL_SLEEP_TOLERANCE);
	constexpr float ang_sleep_tolerance = au::Sqr(P_ANG_SLEEP_TOLERANCE);

	if ((pb.m_flags & PhysicsBody::B_AutoSleep) == 0 ||
		au::Sqr(pb.GetAngularVelocity()) > ang_sleep_tolerance ||
		pb.GetVelocity().LengthSq() > vel_sleep_tolerance)
	{
		pb.m_sleep_time = 0.0f;
	}
	else
	{
		pb.m_sleep_time += m_time->GetRealDT();
	}

	if (pb.m_sleep_time >= 0.5f)
		pb.SetAwake(false);
}

void PhysicsSystem::PreSolve(BodyTransform& bt, BodyLastTransform& blt, const Transform& t) const
{
	// possible because a physics body is not allowed to have a parent
	bt.m_position = blt.m_position = t.GetPosition();
	bt.m_rotation = blt.m_rotation = t.GetRotation();
}

void PhysicsSystem::PostSolve(const BodyTransform& bt, Transform& t) const
{
	t.SetPosition(bt.m_position);
	t.SetRotation(bt.m_rotation);
}
