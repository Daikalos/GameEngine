#include <Velox/Physics/CollisionTable.h>

#include <Velox/Physics/Circle.h>
#include <Velox/Physics/Box.h>
#include <Velox/Physics/CollisionData.h>

#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/Graphics/Components/Transform.h>

using namespace vlx;

/// <summary>
///		
///	 ___Collision Table___
///		 _____________
///		|	C	A	P	X 
///		|C	CC	CA	CP	CX
///		|A	AC	AA	AP	AX
///		|P	PC	PA	PP	PX
///		|X	XC	XA	XP	XX
///   
/// </summary>
CollisionTable::Matrix CollisionTable::table =
{
	&CollisionTable::CircleToCircle,	&CollisionTable::CircleToBox,		&CollisionTable::CircleToPoint,		&CollisionTable::CircleToConvex,
	&CollisionTable::BoxToCircle,		&CollisionTable::BoxToBox,		&CollisionTable::BoxToPoint,		&CollisionTable::BoxToConvex,
	&CollisionTable::PointToCircle,		&CollisionTable::PointToBox,		&CollisionTable::PointToPoint,		&CollisionTable::PointToConvex,
	&CollisionTable::ConvexToCircle,	&CollisionTable::ConvexToBox,		&CollisionTable::ConvexToPoint,		&CollisionTable::ConvexToConvex
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

	const float dist_sqr = normal.lengthSq();
	const float radius = c1.radius + c2.radius;

	if (dist_sqr >= radius * radius)
		return;

	collision.contact_count = 1;

	if (dist_sqr == 0.0f)
	{
		collision.penetration	= c1.radius;
		collision.normal		= sf::Vector2f::UnitX;
		collision.contacts[0]	= t1.GetPosition();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	collision.penetration	= radius - distance;
	collision.normal		= normal / distance;
	collision.contacts[0]	= collision.normal * c1.radius + t1.GetPosition();
}
void CollisionTable::CircleToBox(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	BoxToCircle(collision, s2, t2, s1, t1);
	collision.normal = -collision.normal;
}
void CollisionTable::CircleToPoint(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{

}
void CollisionTable::CircleToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}

void CollisionTable::BoxToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	Box& a1 = reinterpret_cast<Box&>(s1);
	Circle& c2 = reinterpret_cast<Circle&>(s2);


	//sf::Vector2f center = t1.GetPosition();
	//center = mu::Transpose(t2.GetTransform()) * (center - t2.GetPosition());

	sf::Vector2f n = t2.GetPosition() - t1.GetPosition();

	sf::Vector2f closest = n;

	sf::Vector2f half_extends(
		a1.rectangle.Width() / 2.0f,
		a1.rectangle.Height() / 2.0f);

	closest = vu::Clamp(closest, -half_extends, half_extends);

	bool inside = false;

	if (n == closest)
	{
		inside = true;
		if (std::abs(n.x) > std::abs(n.y))
			closest.x = (closest.x > 0.0f) ? half_extends.x : -half_extends.x;
		else
			closest.y = (closest.y > 0.0f) ? half_extends.y : -half_extends.y;
	}

	sf::Vector2f normal = n - closest;
	float dist = normal.lengthSq();
	float r = c2.radius;

	if ((dist > (r * r)) && !inside)
		return;

	dist = std::sqrt(dist);
	n = (normal / dist);

	collision.contact_count = 1;
	collision.penetration	= r - dist;
	collision.normal		= (inside ? -n : n);
	collision.contacts[0]	= closest + t1.GetPosition(); // TODO: FIX
}
void CollisionTable::BoxToBox(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::BoxToPoint(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}
void CollisionTable::BoxToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}

void CollisionTable::PointToCircle(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}

void CollisionTable::PointToBox(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}

void CollisionTable::PointToPoint(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}

void CollisionTable::PointToConvex(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}

void CollisionTable::ConvexToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::ConvexToBox(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
void CollisionTable::ConvexToPoint(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{
}
void CollisionTable::ConvexToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}
