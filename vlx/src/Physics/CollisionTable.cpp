#include <Velox/Physics/CollisionTable.h>

#include <Velox/Physics/Circle.h>
#include <Velox/Physics/CollisionData.h>

#include <Velox/Components/LocalTransform.h>
#include <Velox/Components/Transform.h>

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

void CollisionTable::Collide(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	table[s2.GetType() + s1.GetType() * Shape::Type::Count](collision, s1, t1, s2, t2);
}

void CollisionTable::CircleToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	Circle& c1 = reinterpret_cast<Circle&>(s1); // cast is assumed safe in this kind of context
	Circle& c2 = reinterpret_cast<Circle&>(s2);

	sf::Vector2f normal = t2.GetPosition() - t1.GetPosition();

	const float dist_sqr = vu::DistanceSq(normal);
	const float radius = c1.radius + c2.radius;

	if (dist_sqr >= radius * radius)
		return;

	if (dist_sqr == 0.0f)
	{
		collision.penetration	= c1.radius;
		collision.normal		= sf::Vector2f::UnitX;
		collision.contact_count = 1;

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	collision.penetration	= radius - distance;
	collision.normal		= normal / distance;
	collision.contact_count = 1;
}
void CollisionTable::CircleToAABB(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::CircleToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}

void CollisionTable::AABBToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::AABBToAABB(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::AABBToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}

void CollisionTable::ConvexToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::ConvexToAABB(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::ConvexToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
