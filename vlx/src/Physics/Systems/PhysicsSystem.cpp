#include <Velox/Physics/Systems/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time)
	: SystemAction(entity_admin, id), 

	m_time(&time), 

	m_broad_system(		entity_admin, LYR_BROAD_PHASE),
	m_narrow_system(	entity_admin, LYR_NARROW_PHASE, m_broad_system),

	m_update_forces(	entity_admin, LYR_PHYSICS + 1),
	m_update_positions(	entity_admin, LYR_PHYSICS + 2),
	m_clear_forces(		entity_admin, LYR_PHYSICS + 3),
	m_sleep_bodies(		entity_admin, LYR_PHYSICS + 4),
	m_post_update(		entity_admin, LYR_PHYSICS + 5),
	m_interp(			entity_admin, LYR_PHYSICS + 6)

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

			body.last_pos = body.curr_pos;
			body.last_rot = body.curr_rot;

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
			// TODO: check if body has moved above a certain threshold for the last x seconds, if not, decrement timer, otherwise, set timer to max

			if (body.m_awake)
			{
				body.m_sleep_timer -= m_time->GetRealDT();
				if (body.m_sleep_timer <= 0.0f)
					body.m_awake = false;
			}
		});

	m_post_update.Each([&time](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			body.curr_pos = local_transform.GetPosition();
			body.curr_rot = local_transform.GetRotation();
		});

	m_interp.Each([&time](EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
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

	std::span<CollisionArbiter> arbiters = m_narrow_system.GetArbiters();

	m_update_forces.ForceRun();

	for (auto& collision : arbiters)
		Initialize(collision);

	for (int i = 0; i < m_iterations; ++i)
		for (auto& collision : arbiters)
			ResolveCollision(collision);

	m_update_positions.ForceRun();

	for (auto& collision : arbiters)
		PositionalCorrection(collision);

	m_post_update.ForceRun();

	m_clear_forces.ForceRun();

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

	for (std::uint32_t i = 0; i < arbiter.contacts_count; ++i)
	{
		CollisionContact& contact = arbiter.contacts[i];

		Vector2f ra = contact.position - arbiter.A->shape->GetCenter();
		Vector2f rb = contact.position - arbiter.B->shape->GetCenter();

		{
			float rna = ra.Cross(contact.normal);
			float rnb = rb.Cross(contact.normal);

			float k_normal = AB.GetInvMass() + BB.GetInvMass() +
				AB.GetInvInertia() * au::Sqr(rna) + BB.GetInvInertia() * au::Sqr(rnb);

			contact.mass_normal = 1.0f / k_normal;
		}

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		{
			Vector2f tangent = rv - (contact.normal * rv.Dot(contact.normal));
			tangent = tangent.Normalize();

			float rta = ra.Cross(tangent);
			float rtb = rb.Cross(tangent);

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

		Vector2f ra = contact.position - AS.GetCenter();
		Vector2f rb = contact.position - BS.GetCenter();

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		const float vel_along_normal = rv.Dot(contact.normal);

		if (vel_along_normal > 0.0f) // no need to resolve if they are separating
			return;

		float dpn = (1.0f + arbiter.restitution) * -vel_along_normal * contact.mass_normal;
		dpn = std::max(dpn, 0.0f);
		//dpn /= (float)arbiter.contacts_count;

		Vector2f impulse = dpn * contact.normal;

		AB.ApplyImpulse(-impulse, ra);
		BB.ApplyImpulse( impulse, rb);

		// friction

		rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
			 AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		Vector2f tangent = rv - (contact.normal * rv.Dot(contact.normal));
		tangent = tangent.Normalize();

		float dpt = -rv.Dot(tangent) * contact.mass_tangent ;
		//dpt /= (float)arbiter.contacts_count;

		if (au::Equal(dpt, 0.0f, PHYSICS_EPSILON))
			return;

		const Vector2f friction_impulse = (std::abs(dpt) < dpn * arbiter.static_friction) ?
			(dpt * tangent) : (-dpn * tangent * arbiter.dynamic_friction);

		AB.ApplyImpulse(-friction_impulse, ra);
		BB.ApplyImpulse( friction_impulse, rb);
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