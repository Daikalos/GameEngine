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
			CollisionData&, Shape&,  Transform&, Shape&,  Transform&)>, Shape::Count * Shape::Count>;

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

		static void CircleToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void CircleToBox		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void CircleToPoint	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void CircleToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

		static void BoxToCircle		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void BoxToBox		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void BoxToPoint		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void BoxToConvex		(CollisionData&, Shape&, Transform&, Shape&, Transform&);

		static void PointToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void PointToBox		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void PointToPoint	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void PointToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

		static void ConvexToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void ConvexToBox		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void ConvexToPoint	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void ConvexToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

	private:
		static Matrix table;
	};
}