#pragma once

#include <array>
#include <functional>

#include "Shapes/Shape.h"
#include "Shapes/Circle.h"
#include "Shapes/Box.h"

#include "CollisionArbiter.h"

namespace vlx
{
	class CollisionTable
	{
	public:
		using Matrix = std::array<std::function<void(
			CollisionArbiter&, const Shape&, const Shape&)>, Shape::Count * Shape::Count>;

	public:
		static void Collide(CollisionArbiter&, 
			const Shape& s1, typename Shape::Type st1, 
			const Shape& s2, typename Shape::Type st2);

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

		static void CircleToCircle	(CollisionArbiter&, const Shape&, const Shape&);
		static void CircleToBox		(CollisionArbiter&, const Shape&, const Shape&);
		static void CircleToPoint	(CollisionArbiter&, const Shape&, const Shape&);
		static void CircleToConvex	(CollisionArbiter&, const Shape&, const Shape&);

		static void BoxToCircle		(CollisionArbiter&, const Shape&, const Shape&);
		static void BoxToBox		(CollisionArbiter&, const Shape&, const Shape&);
		static void BoxToPoint		(CollisionArbiter&, const Shape&, const Shape&);
		static void BoxToConvex		(CollisionArbiter&, const Shape&, const Shape&);

		static void PointToCircle	(CollisionArbiter&, const Shape&, const Shape&);
		static void PointToBox		(CollisionArbiter&, const Shape&, const Shape&);
		static void PointToPoint	(CollisionArbiter&, const Shape&, const Shape&);
		static void PointToConvex	(CollisionArbiter&, const Shape&, const Shape&);

		static void ConvexToCircle	(CollisionArbiter&, const Shape&, const Shape&);
		static void ConvexToBox		(CollisionArbiter&, const Shape&, const Shape&);
		static void ConvexToPoint	(CollisionArbiter&, const Shape&, const Shape&);
		static void ConvexToConvex	(CollisionArbiter&, const Shape&, const Shape&);

	private:
		static Matrix table;
	};
}