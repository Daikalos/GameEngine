#include <Velox/Physics/CollisionSolver.h>

#include <Velox/System/SimpleTransform.h>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/WorldManifold.h>
#include <Velox/Physics/CollisionBody.h>
#include <Velox/Physics/CollisionArbiter.h>
#include <Velox/Physics/BodyTransform.h>
#include <Velox/Physics/PhysicsBody.h>

using namespace vlx;

void CollisionSolver::CreateConstraints(std::span<const CollisionArbiter> arbiters)
{
	m_velocity_constraints.clear();
	m_velocity_constraints.resize(arbiters.size());

	for (int32 i = 0; i < arbiters.size(); ++i)
	{
		const LocalManifold& manifold = arbiters[i].manifold;
		VelocityConstraint& vc = m_velocity_constraints[i];

		vc.contacts_count = manifold.contacts_count;
	}
}

void CollisionSolver::SetupConstraints(std::span<const CollisionArbiter> arbiters, const Time& time, const Vector2f& gravity)
{
	for (int32 i = 0; i < arbiters.size(); ++i)
	{
		const CollisionArbiter& arbiter = arbiters[i];
		VelocityConstraint& vc = m_velocity_constraints[i];

		PhysicsBody& AB = *arbiter.A->body;
		PhysicsBody& BB = *arbiter.B->body;

		const BodyTransform& AT = *arbiter.A->transform;
		const BodyTransform& BT = *arbiter.B->transform;

		vc.restitution	= std::min(AB.GetRestitution(), BB.GetRestitution());
		vc.friction		= std::sqrt(AB.GetFriction() * BB.GetFriction());

		// wake up both if either body is dynamic or kinematic
		if (AB.GetType() != BodyType::Static && BB.GetType() != BodyType::Static)
		{
			AB.SetAwake(true);
			BB.SetAwake(true);
		}

		SimpleTransform AW;
		SimpleTransform BW;

		AW.SetRotation(AT.GetRotation());
		BW.SetRotation(BT.GetRotation());

		AW.SetPosition(AT.GetPosition());
		BW.SetPosition(BT.GetPosition());

		WorldManifold manifold;
		manifold.Initialize(arbiter.manifold, 
			AW, arbiter.A->shape->GetRadius(), 
			BW, arbiter.B->shape->GetRadius());

		vc.normal = manifold.normal;

		for (int32 j = 0; j < vc.contacts_count; ++j)
		{
			typename VelocityConstraint::Contact& contact = vc.contacts[j];

			contact.ra = manifold.contacts[j] - AT.GetPosition();
			contact.rb = manifold.contacts[j] - BT.GetPosition();

			{
				float rna = contact.ra.Cross(vc.normal);
				float rnb = contact.rb.Cross(vc.normal);

				float k_normal = AB.GetInvMass() + BB.GetInvMass() +
					AB.GetInvInertia() * au::Sqr(rna) + BB.GetInvInertia() * au::Sqr(rnb);

				contact.mass_normal = (k_normal > 0.0f) ? (1.0f / k_normal) : 0.0f;
			}

			{
				Vector2f tangent = Vector2f::Cross(vc.normal, 1.0f);

				float rta = contact.ra.Cross(tangent);
				float rtb = contact.rb.Cross(tangent);

				float k_tangent = AB.GetInvMass() + BB.GetInvMass() +
					AB.GetInvInertia() * au::Sqr(rta) + BB.GetInvInertia() * au::Sqr(rtb);

				contact.mass_tangent = (k_tangent > 0.0f) ? (1.0f / k_tangent) : 0.0f;
			}

			Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
						  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

			if (rv.LengthSq() < (gravity * time.GetFixedDT()).LengthSq() + FLT_EPSILON)
				vc.restitution = 0.0f;
		}
	}
}

void CollisionSolver::ResolveVelocity(std::span<CollisionArbiter> arbiters)
{
	for (int32 i = 0; i < arbiters.size(); ++i)
	{
		const CollisionArbiter& arbiter = arbiters[i];
		VelocityConstraint& vc = m_velocity_constraints[i];

		PhysicsBody& AB = *arbiter.A->body;
		PhysicsBody& BB = *arbiter.B->body;

		const float am = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvMass() : 0.0f;
		const float bm = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvMass() : 0.0f;

		const float ai = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvInertia() : 0.0f;
		const float bi = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvInertia() : 0.0f;

		Vector2f tangent = Vector2f::Cross(vc.normal, 1.0f);
		
		for (int32 j = 0; j < vc.contacts_count; ++j)
		{
			typename VelocityConstraint::Contact& contact = vc.contacts[j];

			Vector2f rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
						  AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

			const float vel_along_normal = rv.Dot(vc.normal);

			if (vel_along_normal >= 0.0f) // no need to resolve if they are separating
				continue;

			float dpn = -vel_along_normal * (1.0f + vc.restitution) * contact.mass_normal;
			
			float pn0 = contact.impulse_normal;
			contact.impulse_normal = std::max(pn0 + dpn, 0.0f);
			dpn = contact.impulse_normal - pn0;

			const Vector2f pn = vc.normal * dpn;

			if (AB.IsAwake() && AB.IsEnabled())
			{
				AB.m_velocity			-= pn * am;
				AB.m_angular_velocity	-= contact.ra.Cross(pn) * ai;
			}

			if (BB.IsAwake() && BB.IsEnabled())
			{
				BB.m_velocity			+= pn * bm;
				BB.m_angular_velocity	+= contact.rb.Cross(pn) * bi;
			}

			rv = BB.GetVelocity() + Vector2f::Cross(BB.GetAngularVelocity(), contact.rb) -
				 AB.GetVelocity() - Vector2f::Cross(AB.GetAngularVelocity(), contact.ra);

			float dpt = -rv.Dot(tangent) * contact.mass_tangent;

			const float max_pt = vc.friction * contact.impulse_normal;

			const float pt0 = contact.impulse_tangent;
			contact.impulse_tangent = std::clamp(pt0 + dpt, -max_pt, max_pt);
			dpt = contact.impulse_tangent - pt0;

			const Vector2f pt = tangent * dpt;

			if (AB.IsAwake() && AB.IsEnabled())
			{
				AB.m_velocity			-= pt * am;
				AB.m_angular_velocity	-= contact.ra.Cross(pt) * ai;
			}

			if (BB.IsAwake() && BB.IsEnabled())
			{
				BB.m_velocity			+= pt * bm;
				BB.m_angular_velocity	+= contact.rb.Cross(pt) * bi;
			}
		}
	}
}

bool CollisionSolver::ResolvePosition(std::span<CollisionArbiter> arbiters)
{
	float max_penetration = 0.0f;

	for (int32 i = 0; i < arbiters.size(); ++i)
	{
		const CollisionArbiter& arbiter = arbiters[i];
		const LocalManifold& lm = arbiter.manifold;

		const PhysicsBody& AB = *arbiter.A->body;
		const PhysicsBody& BB = *arbiter.B->body;

		BodyTransform& AT = *arbiter.A->transform;
		BodyTransform& BT = *arbiter.B->transform;

		const float am = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvMass() : 0.0f;
		const float bm = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvMass() : 0.0f;

		const float ai = (AB.GetType() == BodyType::Dynamic) ? AB.GetInvInertia() : 0.0f;
		const float bi = (BB.GetType() == BodyType::Dynamic) ? BB.GetInvInertia() : 0.0f;

		for (int32 j = 0; j < lm.contacts_count; ++j)
		{
			SimpleTransform AW;
			SimpleTransform BW;

			AW.SetRotation(AT.GetRotation());
			BW.SetRotation(BT.GetRotation());

			AW.SetPosition(AT.GetPosition());
			BW.SetPosition(BT.GetPosition());

			PositionSolverManifold psm;
			psm.Initialize(lm,
				AW, arbiter.A->shape->GetRadius(), 
				BW, arbiter.B->shape->GetRadius(), j);

			const auto& normal		= psm.normal;
			const auto& contact		= psm.contact;
			const auto& penetration = psm.penetration;

			max_penetration = std::max(max_penetration, penetration);

			const Vector2f ra = contact - AT.GetPosition();
			const Vector2f rb = contact - BT.GetPosition();

			const float correction = std::clamp(P_BAUMGARTE * (penetration - P_SLOP), 0.0f, P_MAX_CORRECTION);

			const float rna = ra.Cross(normal);
			const float rnb = rb.Cross(normal);

			const float k_normal = am + bm + ai * au::Sqr(rna) + bi * au::Sqr(rnb);
			const float impulse = (k_normal > 0.0f) ? (correction / k_normal) : 0.0f;

			const Vector2f p = normal * impulse;

			if (AB.IsAwake() && AB.IsEnabled())
			{
				AT.m_position -= p * am;
				AT.m_rotation -= sf::radians(ai * ra.Cross(p));
			}

			if (BB.IsAwake() && BB.IsEnabled())
			{
				BT.m_position += p * bm;
				BT.m_rotation += sf::radians(bi * rb.Cross(p));
			}
		}
	}

	return max_penetration <= 3.0f * P_SLOP;
}

void PositionSolverManifold::Initialize(const LocalManifold& manifold, const SimpleTransform& AW, float AR, const SimpleTransform& BW, float BR, std::size_t index)
{
	assert(manifold.contacts_count > 0);

	switch (manifold.type)
	{
	case LocalManifold::Type::Circles:
		{
			Vector2f a_center = AW.GetPosition();
			Vector2f b_center = BW.GetPosition();

			Vector2f dir = Vector2f::Direction(a_center, b_center);
			normal = dir.Normalize();

			contact		= 0.5f * (a_center + b_center);
			penetration = -(dir.Dot(normal) - AR - BR);
		}
		break;
	case LocalManifold::Type::FaceA:
		{
			normal = AW.GetRotation().Transform(manifold.normal);
			Vector2f plane_contact = AW.Transform(manifold.point);

			Vector2f clip_contact = BW.Transform(manifold.contacts[index].point);
			penetration = -(Vector2f::Direction(plane_contact, clip_contact).Dot(normal) - AR - BR);
			contact = clip_contact;
		}
		break;
	case LocalManifold::Type::FaceB:
		{
			normal = BW.GetRotation().Transform(manifold.normal);
			Vector2f plane_contact = BW.Transform(manifold.point);

			Vector2f clip_contact = AW.Transform(manifold.contacts[index].point);
			penetration = -(Vector2f::Direction(plane_contact, clip_contact).Dot(normal) - AR - BR);
			contact = clip_contact;

			normal = -normal;
		}
		break;
	case LocalManifold::Type::None:
	default:
		throw std::runtime_error("Invalid type");
	}
}
