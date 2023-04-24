#include <Velox/Physics/CollisionArbiter.h>

using namespace vlx;

void CollisionArbiter::Initialize(const Time& time, const Vector2f& gravity)
{
	PhysicsBody& AB = *A;
	PhysicsBody& BB = *B;

	restitution = std::min(AB.GetRestitution(), BB.GetRestitution());
	friction = std::sqrt(AB.GetFriction() * BB.GetFriction());

	// wake up both if either body is dynamic or kinematic
	if (AB.GetType() != BodyType::Static && BB.GetType() != BodyType::Static)
	{
		AB.SetAwake(true);
		BB.SetAwake(true);
	}

	for (uint32 i = 0; i < contacts_count; ++i)
	{
		CollisionContact& contact = contacts[i];

		Vector2f ra = contact.position - AB.position;
		Vector2f rb = contact.position - BB.position;

		{
			float rna = ra.Cross(contact.normal);
			float rnb = rb.Cross(contact.normal);

			float k_normal = AB.GetInvMass() + BB.GetInvMass() +
				AB.GetInvInertia() * au::Sqr(rna) + BB.GetInvInertia() * au::Sqr(rnb);

			contact.mass_normal = (k_normal > 0.0f) ? (1.0f / k_normal) : 0.0f;
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

			contact.mass_tangent = (k_tangent > 0.0f) ? (1.0f / k_tangent) : 0.0f;
		}

		if (rv.LengthSq() < (gravity * time.GetFixedDT()).LengthSq() + FLT_EPSILON)
			restitution = 0.0f;
	}
}

void CollisionArbiter::ResolveVelocity()
{
	PhysicsBody& AB = *A;
	PhysicsBody& BB = *B;

	float am = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvMass() : 0.0f;
	float bm = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvMass() : 0.0f;

	float ai = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvInertia() : 0.0f;
	float bi = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvInertia() : 0.0f;

	for (std::size_t i = 0; i < contacts_count; ++i)
	{
		CollisionContact& contact = contacts[i];

		Vector2f ra = contact.position - AB.position;
		Vector2f rb = contact.position - BB.position;

		Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
					  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		const float vel_along_normal = rv.Dot(contact.normal);

		if (vel_along_normal >= 0.0f) // no need to resolve if they are separating
			continue;

		float dpn = -vel_along_normal * (1.0f + restitution) * contact.mass_normal;
		//dpn = std::max(dpn, 0.0f);

		float pn0 = contact.pn;
		contact.pn = std::max(pn0 + dpn, 0.0f);
		dpn = contact.pn - pn0;

		Vector2f impulse = contact.normal * dpn;

		if (AB.IsAwake() && AB.IsEnabled())
		{
			AB.m_velocity			-= impulse * am;
			AB.m_angular_velocity	-= ra.Cross(impulse) * ai;
		}

		if (BB.IsAwake() && BB.IsEnabled())
		{
			BB.m_velocity			+= impulse * bm;
			BB.m_angular_velocity	+= rb.Cross(impulse) * bi;
		}

		// friction

		rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), rb) -
			 AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), ra);

		Vector2f tangent = rv - (contact.normal * rv.Dot(contact.normal));
		tangent = tangent.Normalize();

		float dpt = -rv.Dot(tangent) * contact.mass_tangent;

		float max_pt = friction * contact.pn;

		float pt0 = contact.pt;
		contact.pt = std::clamp(pt0 + dpt, -max_pt, max_pt);
		dpt = contact.pt - pt0;

		const Vector2f friction_impulse = tangent * dpt;

		if (AB.IsAwake() && AB.IsEnabled())
		{
			AB.m_velocity			-= friction_impulse * am;
			AB.m_angular_velocity	-= ra.Cross(friction_impulse) * ai;
		}

		if (BB.IsAwake() && BB.IsEnabled())
		{
			BB.m_velocity			+= friction_impulse * bm;
			BB.m_angular_velocity	+= rb.Cross(friction_impulse) * bi;
		}
	}
}

void CollisionArbiter::ResolvePosition()
{
	PhysicsBody& AB = *A;
	PhysicsBody& BB = *B;

	float am = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvMass() : 0.0f;
	float bm = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvMass() : 0.0f;

	//float ai = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvInertia() : 0.0f;
	//float bi = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvInertia() : 0.0f;

	for (std::size_t i = 0; i < contacts_count; ++i)
	{
		CollisionContact& contact = contacts[i];

		Vector2f ra = contact.position - AB.position;
		Vector2f rb = contact.position - BB.position;

		//Vector2f correction = (std::max(contact.penetration - P_SLOP, 0.0f) /
		//	(am + bm)) * contact.normal * P_PERCENT;

		float correction = std::max(contact.penetration - P_SLOP, 0.0f);

		//float rna = ra.Cross(contact.normal);
		//float rnb = rb.Cross(contact.normal);

		float k_normal = am + bm; // +ai * au::Sqr(rna) + bi * au::Sqr(rnb);
		float impulse = (k_normal > 0.0f) ? (correction / k_normal) : 0.0f;

		Vector2f p = contact.normal * impulse * P_PERCENT;

		if (AB.IsAwake() && AB.IsEnabled())
		{
			AB.position -= p * am;
			//AB.rotation -= sf::radians(ai * ra.Cross(p));
		}

		if (BB.IsAwake() && BB.IsEnabled())
		{
			BB.position += p * bm;
			//BB.rotation += sf::radians(bi * rb.Cross(p));
		}
	}
}
