#pragma once

#include <array>
#include <functional>

#include "Shapes/Shape.h"
#include "CollisionData.h"

namespace vlx
{
	struct CollisionData;

	class CollisionTable
	{
	public:
		using Matrix = std::array<std::function<void(
			CollisionData&, const Shape&, const Shape&)>, Shape::Count * Shape::Count>;

	public:
		static void Collide(CollisionData&, 
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

		static void CircleToCircle	(CollisionData&, const Shape&, const Shape&);
		static void CircleToBox		(CollisionData&, const Shape&, const Shape&);
		static void CircleToPoint	(CollisionData&, const Shape&, const Shape&);
		static void CircleToConvex	(CollisionData&, const Shape&, const Shape&);

		static void BoxToCircle		(CollisionData&, const Shape&, const Shape&);
		static void BoxToBox		(CollisionData&, const Shape&, const Shape&);
		static void BoxToPoint		(CollisionData&, const Shape&, const Shape&);
		static void BoxToConvex		(CollisionData&, const Shape&, const Shape&);

		static void PointToCircle	(CollisionData&, const Shape&, const Shape&);
		static void PointToBox		(CollisionData&, const Shape&, const Shape&);
		static void PointToPoint	(CollisionData&, const Shape&, const Shape&);
		static void PointToConvex	(CollisionData&, const Shape&, const Shape&);

		static void ConvexToCircle	(CollisionData&, const Shape&, const Shape&);
		static void ConvexToBox		(CollisionData&, const Shape&, const Shape&);
		static void ConvexToPoint	(CollisionData&, const Shape&, const Shape&);
		static void ConvexToConvex	(CollisionData&, const Shape&, const Shape&);

	private:
		static Matrix table;
	};
}