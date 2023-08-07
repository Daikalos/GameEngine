#pragma once

#include <array>
#include <functional>
#include <span>
#include <tuple>

#include <Velox/System/SimpleTransform.h>

#include "../Shapes/Shape.h"
#include "../Shapes/Circle.h"
#include "../Shapes/Box.h"
#include "../Shapes/Polygon.h"
#include "../Shapes/Point.h"

#include "CollisionArbiter.h"

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API CollisionTable
	{
	public:
		using Matrix = std::array<std::function<void(CollisionArbiter&, 
			const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&)>, Shape::Count * Shape::Count>;

		using Face = std::array<Vector2f, 2>;
		using VectorSpan = std::span<const Vector2f>;

	public:
		static void Collide(CollisionArbiter&, 
			const Shape&, const SimpleTransform&, typename Shape::Type,
			const Shape&, const SimpleTransform&, typename Shape::Type);

	private:
		/// <summary>
		///		
		///	 ___Collision Table___
		///		 _____________
		///		|	C	B	P	X 
		///		|C	CC	CB	CP	CX
		///		|B	BC	BB	BP	BX
		///		|P	PC	PB	PP	PX
		///		|X	XC	XB	XP	XX
		///   
		/// </summary>

		static void CircleToCircle	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void CircleToBox		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void CircleToPoint	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void CircleToConvex	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static void BoxToCircle		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void BoxToBox		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void BoxToPoint		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void BoxToConvex		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static void PointToCircle	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void PointToBox		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void PointToPoint	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void PointToConvex	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static void ConvexToCircle	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void ConvexToBox		(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void ConvexToPoint	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static void ConvexToConvex	(CollisionArbiter&, const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

	private:
		static void CircleToPolygon(CollisionArbiter& arbiter, 
			const Vector2f& pos1, float radius1,
			const SimpleTransform& t2, float radius2, VectorSpan vs2, VectorSpan ns2);

		static void PolygonToPolygon(CollisionArbiter& arbiter,
			const SimpleTransform& t1, float radius1, VectorSpan vs1, VectorSpan ns1,
			const SimpleTransform& t2, float radius2, VectorSpan vs2, VectorSpan ns2);

	private:
		static constexpr bool BiasGreaterThan(float a, float b);

	private:
		static Vector2f GetSupport(VectorSpan vertices, const Vector2f& dir);

		static std::tuple<float, uint32_t> FindAxisLeastPenetration(
			const SimpleTransform& t1, VectorSpan vs1, VectorSpan ns1,
			const SimpleTransform& t2, VectorSpan vs2, VectorSpan ns2);

		static auto FindIncidentFace(
			const SimpleTransform& t1, VectorSpan vs1, VectorSpan ns1,
			const SimpleTransform& t2, const Vector2f& n2) -> Face;

		static int Clip(Face& face, const Vector2f& n, float c);

	private:
		static Matrix table;
	};

	constexpr bool CollisionTable::BiasGreaterThan(float a, float b)
	{
		constexpr float k_bias_relative = 0.95f;
		constexpr float k_bias_absolute = 0.01f;

		return a >= b * k_bias_relative + a * k_bias_absolute;
	}
}