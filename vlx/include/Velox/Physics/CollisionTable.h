#pragma once

#include <array>
#include <functional>

#include "Shape.h"
#include "CollisionResult.h"

namespace vlx
{
	struct Circle;
	struct Rectangle;
	struct CollisionResult;

	class CollisionTable
	{
	public:
		using Matrix = std::array<std::function<CollisionResult(Shape&, Shape&)>, Shape::Count * Shape::Count>;

	public:
		static CollisionResult Collide(Shape& s1, Shape& s2);

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

		static CollisionResult CircleToCircle(Shape& s1, Shape& s2);
		static CollisionResult CircleToAABB(Shape& s1, Shape& s2);
		static CollisionResult CircleToConvex(Shape& s1, Shape& s2);

		static CollisionResult AABBToCircle(Shape& s1, Shape& s2);
		static CollisionResult AABBToAABB(Shape& s1, Shape& s2);
		static CollisionResult AABBToConvex(Shape& s1, Shape& s2);

		static CollisionResult ConvexToCircle(Shape& s1, Shape& s2);
		static CollisionResult ConvexToAABB(Shape& s1, Shape& s2);
		static CollisionResult ConvexToConvex(Shape& s1, Shape& s2);

	private:
		static Matrix table;
	};
}