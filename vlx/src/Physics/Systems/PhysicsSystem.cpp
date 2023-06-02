#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time)
	: SystemAction(entity_admin, id), 

	m_time(&time), 

	m_broad_system(			entity_admin, LYR_BROAD_PHASE),
	m_narrow_system(		entity_admin, LYR_NARROW_PHASE),

	m_integrate_velocity(	entity_admin),
	m_integrate_position(	entity_admin),
	m_sleep_bodies(			entity_admin),
	m_pre_solve(			entity_admin),
	m_post_solve(			entity_admin)

{
	m_integrate_velocity.Each(
		[this](EntityID entity_id, PhysicsBody& body)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() != BodyType::Dynamic)
				return;

			// velocity
			body.m_velocity			+= m_time->GetFixedDT() * body.GetInvMass() * (body.GetGravityScale() * body.GetMass() * m_gravity + body.GetForce());
			body.m_angular_velocity += m_time->GetFixedDT() * body.GetInvInertia() * body.GetTorque();

			// damping
			body.m_velocity			*= (1.0f / (1.0f + m_time->GetFixedDT() * body.GetLinearDamping()));
			body.m_angular_velocity *= (1.0f / (1.0f + m_time->GetFixedDT() * body.GetAngularDamping()));
		});

	m_integrate_position.Each(
		[this](EntityID entity_id, PhysicsBody& body)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			body.position += body.GetVelocity() * m_time->GetFixedDT();
			body.rotation += sf::radians(body.GetAngularVelocity() * m_time->GetFixedDT());

			body.m_force = Vector2f::Zero;
			body.m_torque = 0.0f;
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
		[this](EntityID entity_id, PhysicsBody& body, Transform& transform)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			body.last_pos = body.position = transform.GetPosition(); // possible because a physics body is not allowed to have a parent
			body.last_rot = body.rotation = transform.GetRotation();
		});

	m_post_solve.Each(
		[this](EntityID entity_id, PhysicsBody& body, Transform& transform)
		{
			if (!body.IsAwake() || !body.IsEnabled())
				return;

			if (body.GetType() == BodyType::Static)
				return;

			transform.SetPosition(body.position); // synchronize the transform
			transform.SetRotation(body.rotation);
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
	m_narrow_system.Update(m_broad_system);

	auto arbiters = m_narrow_system.GetArbiters();

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

	m_post_solve.ForceRun();

	m_sleep_bodies.ForceRun();
}

void PhysicsSystem::PostUpdate()
{

}