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

#include "LocalManifold.h"

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API CollisionTable
	{
	public:
		using Matrix = std::array<std::function<LocalManifold(
			const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&)>, Shape::Count * Shape::Count>;

		using Face = std::array<Vector2f, 2>;
		using VectorSpan = std::span<const Vector2f>;

	public:
		static LocalManifold Collide(
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

		static LocalManifold CircleToCircle	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold CircleToBox	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold CircleToPoint	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold CircleToConvex	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static LocalManifold BoxToCircle	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold BoxToBox		(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold BoxToPoint		(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold BoxToConvex	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static LocalManifold PointToCircle	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold PointToBox		(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold PointToPoint	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold PointToConvex	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

		static LocalManifold ConvexToCircle	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold ConvexToBox	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold ConvexToPoint	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);
		static LocalManifold ConvexToConvex	(const Shape&, const SimpleTransform&, const Shape&, const SimpleTransform&);

	private:
		static LocalManifold CircleToPolygon(
			const Vector2f& pos1, float radius1,
			const SimpleTransform& t2, float radius2, VectorSpan vs2, VectorSpan ns2);

		static LocalManifold PolygonToPolygon(
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