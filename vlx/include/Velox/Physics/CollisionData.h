#pragma once

#include <Velox/System/Vector2.hpp>

namespace vlx
{
	class PhysicsBody;
	class LocalTransform;
	class Transform;

	struct CollisionData
	{
		PhysicsBody*	A				{nullptr};
		PhysicsBody*	B				{nullptr};
		LocalTransform* ALT				{nullptr};
		LocalTransform* BLT				{nullptr};
		Transform*		AT				{nullptr};
		Transform*		BT				{nullptr};

		float			penetration		{0.0f};
		Vector2f		normal;

		Vector2f		contacts[2];
		std::uint32_t	contact_count	{0};

		float			min_restitution	{0.0f};
		float			avg_sfriction	{0.0f};
		float			avg_dfriction	{0.0f};
	};
}