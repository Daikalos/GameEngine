#pragma once

namespace vlx
{
	struct CollisionContact
	{
		Vector2f	position;
		Vector2f	normal;
		Vector2f	ra, rb;
		float		penetration		{0.0f};
		float		mass_normal		{0.0f};
		float		mass_tangent	{0.0f};
	};
}