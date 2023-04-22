#pragma once

#include <array>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Time.h>
#include <Velox/VeloxTypes.hpp>

#include "PhysicsBody.h"
#include "CollisionContact.h"

namespace vlx
{
	class CollisionArbiter
	{
	public:
		using ContactArray = std::array<CollisionContact, 2>;

		PhysicsBody*		A					{nullptr};
		PhysicsBody*		B					{nullptr};

		ContactArray		contacts;
		uint8				contacts_count		{0};
			
		float				restitution			{0.0f};	// minimum TODO: allow user to change mode: min, max, or average
		float				friction			{0.0f};	// average

		void Initialize(const Time& time, const Vector2f& gravity);
		void ResolveVelocity();
		void ResolvePosition();
	};
}