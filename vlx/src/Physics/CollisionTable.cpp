#include <Velox/Physics/CollisionTable.h>

using namespace vlx;

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
CollisionTable::Matrix CollisionTable::table =
{
	&CollisionTable::CircleToCircle,	&CollisionTable::CircleToAABB,		&CollisionTable::CircleToConvex,
	&CollisionTable::AABBToCircle,		&CollisionTable::AABBToAABB,		&CollisionTable::AABBToConvex,
	&CollisionTable::ConvexToCircle,	&CollisionTable::ConvexToAABB,		&CollisionTable::ConvexToConvex
};

CollisionResult CollisionTable::Collide(Shape& s1, Shape& s2)
{
	return table[s1.GetType() + s2.GetType() * Shape::Type::Count](s1, s2);
}

CollisionResult CollisionTable::CircleToCircle(Shape& s1, Shape& s2)
{
	Circle& c1 = (Circle&)s1; // cast is assumed safe in this kind of context
	Circle& c2 = (Circle&)s2;



	return CollisionResult();
}
CollisionResult CollisionTable::CircleToAABB(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
CollisionResult CollisionTable::CircleToConvex(Shape& s1, Shape& s2)
{
	return CollisionResult();
}

CollisionResult CollisionTable::AABBToCircle(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
CollisionResult CollisionTable::AABBToAABB(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
CollisionResult CollisionTable::AABBToConvex(Shape& s1, Shape& s2)
{
	return CollisionResult();
}

CollisionResult CollisionTable::ConvexToCircle(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
CollisionResult CollisionTable::ConvexToAABB(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
CollisionResult CollisionTable::ConvexToConvex(Shape& s1, Shape& s2)
{
	return CollisionResult();
}
