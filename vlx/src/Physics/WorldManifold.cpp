#include <Velox/Physics/WorldManifold.h>

#include <Velox/Physics/LocalManifold.h>
#include <Velox/System/SimpleTransform.h>

using namespace vlx;

void WorldManifold::Initialize(const LocalManifold& manifold,
	const SimpleTransform& AW, float AR, 
	const SimpleTransform& BW, float BR)
{
	if (manifold.contacts_count == 0)
		return;

	switch (manifold.type)
	{
	case LocalManifold::Type::Circles:
		{
			normal = Vector2f::UnitX;

			Vector2f a_center = AW.GetPosition();
			Vector2f b_center = BW.GetPosition();

			Vector2f dir = Vector2f::Direction(a_center, b_center);
			float dir_length_sqr = dir.LengthSq();

			if (dir_length_sqr > FLT_EPSILON * FLT_EPSILON)
				normal = dir.Normalize(std::sqrt(dir_length_sqr), 1.0f);

			Vector2f a_contact = a_center + normal * AR;
			Vector2f b_contact = b_center - normal * BR;

			contacts[0]		= 0.5f * (a_contact + b_contact);
			penetrations[0] = -Vector2f::Direction(a_contact, b_contact).Dot(normal);
		}
		break;
	case LocalManifold::Type::FaceA:
		{
			normal = AW.GetRotation().Transform(manifold.normal);
			Vector2f plane_contact = AW.Transform(manifold.point);

			for (int32 i = 0; i < manifold.contacts_count; ++i)
			{
				Vector2f clip_contact = BW.Transform(manifold.contacts[i].point);

				Vector2f a_contact = clip_contact + (AR - Vector2f::Direction(plane_contact, clip_contact).Dot(normal)) * normal;
				Vector2f b_contact = clip_contact - normal * BR;

				contacts[i]		= 0.5f * (a_contact + b_contact);
				penetrations[i] = -Vector2f::Direction(a_contact, b_contact).Dot(normal);
			}
		}
		break;
	case LocalManifold::Type::FaceB:
		{
			normal = BW.GetRotation().Transform(manifold.normal);
			Vector2f plane_contact = BW.Transform(manifold.point);

			for (int32 i = 0; i < manifold.contacts_count; ++i)
			{
				Vector2f clip_contact = AW.Transform(manifold.contacts[i].point);

				Vector2f b_contact = clip_contact + (BR - Vector2f::Direction(plane_contact, clip_contact).Dot(normal)) * normal;
				Vector2f a_contact = clip_contact - normal * AR;

				contacts[i]		= 0.5f * (a_contact + b_contact);
				penetrations[i] = -Vector2f::Direction(b_contact, a_contact).Dot(normal);
			}

			normal = -normal;
		}
		break;
	case LocalManifold::Type::None:
	default:
		throw std::runtime_error("Invalid type");
	}
}
