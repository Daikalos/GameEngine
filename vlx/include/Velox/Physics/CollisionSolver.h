#pragma once

#include <vector>
#include <array>
#include <span>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Time.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class CollisionArbiter;
	class SimpleTransform;
	class LocalManifold;

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
	private:
		using ArbiterSpan = std::span<const CollisionArbiter>;
		using ManifoldSpan = std::span<const LocalManifold>;

	public:
		void CreateConstraints(ArbiterSpan arbiters, ManifoldSpan manifolds);
		void SetupConstraints(ArbiterSpan arbiters, ManifoldSpan manifolds, const Time& time, const Vector2f& gravity);

		void ResolveVelocity(ArbiterSpan arbiters);
		bool ResolvePosition(ArbiterSpan arbiters, ManifoldSpan manifolds);

	private:
		std::vector<VelocityConstraint> m_velocity_constraints;
	};
}