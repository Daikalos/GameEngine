#pragma once

namespace vlx
{
	enum class BodyType
	{
		Static,
		Dynamic,
		Kinematic
	};

	struct PhysicsBody
	{
		float		restitution;
		float		mass;
		BodyType	type;
	};
}