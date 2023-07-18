#pragma once

#include <vector>
#include <array>
#include <span>

#include <Velox/System/SimpleTransform.h>
#include <Velox/System/Vector2.hpp>
#include <Velox/System/Time.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "CollisionArbiter.h"

namespace vlx
{
	struct VELOX_API VelocityConstraint
	{
		struct Contact
		{
			Vector2f	ra;
			Vector2f	rb;
			float		mass_normal		{0.0f};
			float		mass_tangent	{0.0f};
			float		impulse_normal	{0.0f};
			float		impulse_tangent	{0.0f};
		};

		std::array<Contact, 2>	contacts;
		Vector2f				normal;
		float					restitution		{0.0f};	// minimum TODO: allow user to change mode: min, max, or average
		float					friction		{0.0f};	// average
		int32					contacts_count	{0};
	};

	struct VELOX_API PositionSolverManifold
	{
		void Initialize(const LocalManifold& manifold, const SimpleTransform& AW, float AR, const SimpleTransform& BW, float BR, std::size_t index);

		Vector2f	normal;
		Vector2f	contact;
		float		penetration	{0.0f};
	};

	class VELOX_API CollisionSolver
	{
	public:
		void CreateConstraints(std::span<const CollisionArbiter> arbiters);
		void SetupConstraints(std::span<const CollisionArbiter> arbiters, const Time& time, const Vector2f& gravity);

		void ResolveVelocity(std::span<CollisionArbiter> arbiters);
		bool ResolvePosition(std::span<CollisionArbiter> arbiters);

	private:
		std::vector<VelocityConstraint> m_velocity_constraints;
	};
}