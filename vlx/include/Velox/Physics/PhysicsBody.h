#pragma once

#include "MassData.h"
#include "PhysicsMaterial.h"

namespace vlx
{
	enum class BodyType : unsigned char
	{
		Static,
		Dynamic,
		Kinematic
	};

	struct PhysicsBody
	{
		PhysicsMaterial	material;
		MassData		mass_data;
		BodyType		type			{BodyType::Dynamic};
		float			gravity_scale	{1.0f};
	};
}