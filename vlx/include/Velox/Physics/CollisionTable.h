#pragma once

#include <array>
#include <functional>

#include "Shapes/Shape.h"
#include "CollisionData.h"

namespace vlx
{
	struct CollisionData;
	class LocalTransform;
	class Transform;

	class CollisionTable
	{
	public:
		using Matrix = std::array<std::function<void(
			CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&)>, Shape::Count * Shape::Count>;

	public:
		static void Collide(CollisionData&, CollisionObject& a, CollisionObject& b);

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

		static void CircleToCircle	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void CircleToBox		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void CircleToPoint	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void CircleToConvex	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);

		static void BoxToCircle		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void BoxToBox		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void BoxToPoint		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void BoxToConvex		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);

		static void PointToCircle	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void PointToBox		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void PointToPoint	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void PointToConvex	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);

		static void ConvexToCircle	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void ConvexToBox		(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void ConvexToPoint	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);
		static void ConvexToConvex	(CollisionData&, const Shape&, const Transform&, const Shape&, const Transform&);

	private:
		static Matrix table;
	};
}