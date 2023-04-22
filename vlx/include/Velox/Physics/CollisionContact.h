#pragma once

#include <Velox/System/Vector2.hpp>

namespace vlx
{
	struct CollisionContact
	{
		Vector2f	position;
		Vector2f	normal;
		float		penetration		{0.0f};
		float		mass_normal		{0.0f};
		float		mass_tangent	{0.0f};

		float		pn				{0.0f};
		float		pt				{0.0f};
	};
}