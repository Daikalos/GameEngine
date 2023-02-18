#pragma once

#include <array>
#include <functional>

#include "Shape.h"
#include "CollisionData.h"

namespace vlx
{
	struct Circle;
	struct Rectangle;
	struct CollisionData;

	struct PhysicsBody;
	class LocalTransform;
	class Transform;

	class CollisionTable
	{
	public:
		using Matrix = std::array<std::function<void(
			CollisionData&, Shape&,  Transform&, Shape&,  Transform&)>, Shape::Count * Shape::Count>;

	public:
		static void Collide(CollisionData&, Shape&, Transform&, Shape&, Transform&);

	private:
		/// <summary>
		///		
		///	 ___Collision Table___
		///		 _____________
		///		|	C	A	X 
		///		|C	CC	CA	CX
		///		|A	AC	AA	AX
		///		|X	XC	XA	XX
		///   
		/// </summary>

		static void CircleToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void CircleToAABB	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void CircleToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

		static void AABBToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void AABBToAABB		(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void AABBToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

		static void ConvexToCircle	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void ConvexToAABB	(CollisionData&, Shape&, Transform&, Shape&, Transform&);
		static void ConvexToConvex	(CollisionData&, Shape&, Transform&, Shape&, Transform&);

	private:
		static Matrix table;
	};
}