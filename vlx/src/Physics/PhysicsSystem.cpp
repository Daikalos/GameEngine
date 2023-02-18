#include <Velox/Physics/PhysicsSystem.h>

using namespace vlx;

PhysicsSystem::PhysicsSystem(EntityAdmin& entity_admin, const LayerType id, Time& time)
	: SystemObject(entity_admin, id), m_time(&time),
	m_update_forces(entity_admin, LYR_PHYSICS + 1),
	m_update_velocities(entity_admin, LYR_PHYSICS + 2),
	m_clear_forces(entity_admin, LYR_PHYSICS + 3)
{
	m_update_forces.Each([this](const EntityID entity_id, PhysicsBody& body)
		{
			if (body.GetInvMass() == 0.0f)
				return;

			body.AddVelocity((body.GetForce() * body.GetInvMass() + m_gravity) * (m_time->GetFixedDT() / 2.0f));
			body.AddAngularVelocity(body.GetTorque() * body.GetInvInertia() * (m_time->GetFixedDT() / 2.0f));
		});

	m_update_velocities.Each([this](const EntityID entity_id, PhysicsBody& body, LocalTransform& local_transform)
		{
			if (body.GetInvMass() == 0.0f)
				return;

			local_transform.Move(body.GetVelocity() * m_time->GetFixedDT());
			local_transform.Rotate(sf::radians(body.GetAngularVelocity() * m_time->GetFixedDT()));

			body.AddVelocity((body.GetForce() * body.GetInvMass() + m_gravity) * (m_time->GetFixedDT() / 2.0f));
			body.AddAngularVelocity(body.GetTorque() * body.GetInvInertia() * (m_time->GetFixedDT() / 2.0f));
		});

	m_clear_forces.Each([this](const EntityID entity_id, PhysicsBody& body)
		{
			body.SetForce({});
			body.SetTorque(0.0f);
		});
}

void PhysicsSystem::PreUpdate()
{

}

void PhysicsSystem::Update()
{
	// TODO: polygons, add area of influence, perform detailed collision test if the centroid is within the influence area http://wscg.zcu.cz/wscg2004/Papers_2004_Full/B83.pdf
}

void PhysicsSystem::FixedUpdate()
{
	m_collisions_data.clear();

	auto entities = m_entity_admin->GetEntitiesWith<PhysicsBody>(); // naive implementation for now

	for (int i = 0; i < entities.size(); ++i)
		for (int j = 0; j < entities.size(); ++j)
		{
			PhysicsBody& A = m_entity_admin->GetComponent<PhysicsBody>(entities[i]);
			PhysicsBody& B = m_entity_admin->GetComponent<PhysicsBody>(entities[j]);
		}

	m_update_forces.ForceRun();
	
	for (auto& collision : m_collisions_data)
		Initialize(collision);

	for (auto& collision : m_collisions_data)
		ResolveCollision(collision);

	m_update_velocities.ForceRun();

	for (auto& collision : m_collisions_data)
		PositionalCorrection(collision);

	m_clear_forces.ForceRun();
}

void PhysicsSystem::PostUpdate()
{
}

void PhysicsSystem::Initialize(CollisionData& collision)
{
	if (collision.A == nullptr || collision.B == nullptr)
		return;

	PhysicsBody& A = *collision.A;
	PhysicsBody& B = *collision.B;

	collision.min_restitution = std::min(A.GetRestitution(), B.GetRestitution());

	collision.avg_sfriction = std::sqrt(A.GetStaticFriction() * B.GetStaticFriction());
	collision.avg_dfriction = std::sqrt(A.GetDynamicFriction() * B.GetDynamicFriction());

	for (std::uint32_t i = 0; i < collision.contact_count; ++i)
	{
		sf::Vector2f ra = collision.contacts[i] - collision.AT->GetPosition();
		sf::Vector2f rb = collision.contacts[i] - collision.BT->GetPosition();

		sf::Vector2f rv =	B.GetVelocity() + vu::Cross(B.GetAngularVelocity(), rb) -
							A.GetVelocity() - vu::Cross(A.GetAngularVelocity(), ra);

		if (rv.lengthSq() < (m_time->GetFixedDT() * m_gravity).lengthSq() + FLT_EPSILON)
			collision.min_restitution = 0.0f;
	}
}

void PhysicsSystem::ResolveCollision(CollisionData& collision)
{
	if (collision.A == nullptr || collision.B == nullptr)
		return;

	PhysicsBody& A = *collision.A;
	PhysicsBody& B = *collision.B;

	if (au::Equal(A.GetInvMass() + B.GetInvMass(), 0.0f))
	{
		A.SetVelocity({});
		B.SetVelocity({});
		return;
	}

	for (std::size_t i = 0; i < collision.contact_count; ++i)
	{
		sf::Vector2f ra = collision.contacts[i] - collision.AT->GetPosition();
		sf::Vector2f rb = collision.contacts[i] - collision.BT->GetPosition();

		sf::Vector2f rv =	B.GetVelocity() + vu::Cross(B.GetAngularVelocity(), rb) -
							A.GetVelocity() - vu::Cross(A.GetAngularVelocity(), ra);

		const float vel_along_normal = vu::Dot(rv, collision.normal);

		if (vel_along_normal > 0.0f) // no need to resolve if they are separating
			return;

		const float ra_cross_n = vu::Cross(ra, collision.normal);
		const float rb_cross_n = vu::Cross(rb, collision.normal);

		const float inv_mass_sum = A.GetInvMass() + B.GetInvMass() +
			std::sqrt(ra_cross_n) * A.GetInvInertia() + std::sqrt(rb_cross_n) * B.GetInvInertia();

		// impulse scalar
		float j = -(1.0f + collision.min_restitution) * vel_along_normal;
		j /= inv_mass_sum;
		j /= (float)collision.contact_count;

		const sf::Vector2f impulse = j * collision.normal;
		A.ApplyImpulse(-impulse, ra);
		B.ApplyImpulse(impulse, rb);



		rv =	B.GetVelocity() + vu::Cross(B.GetAngularVelocity(), rb) -
				A.GetVelocity() - vu::Cross(A.GetAngularVelocity(), ra);

		sf::Vector2f t = rv - (collision.normal * vu::Dot(rv, collision.normal));
		t = vu::Normalize(t);

		float jt = -vu::Dot(rv, t);
		jt /= inv_mass_sum;
		jt /= (float)collision.contact_count;

		if (au::Equal(jt, 0.0f))
			return;

		const sf::Vector2f tangent_impulse = (std::abs(jt) < j * collision.avg_sfriction) ? t * jt : t * -j * collision.avg_dfriction;

		A.ApplyImpulse(-tangent_impulse, ra);
		B.ApplyImpulse( tangent_impulse, rb);
	}
}

void PhysicsSystem::PositionalCorrection(CollisionData& collision)
{
	const float percent = 0.4f;
	const float k_slop = 0.05f;

	sf::Vector2f correction = std::max(collision.penetration - k_slop, 0.0f) / (collision.A->GetInvMass() + collision.B->GetInvMass()) * collision.normal * percent;

	collision.ALT->Move(-collision.A->GetInvMass() * correction);
	collision.BLT->Move(collision.B->GetInvMass() * correction);
}
