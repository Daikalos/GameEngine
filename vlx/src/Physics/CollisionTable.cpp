#include <Velox/Physics/CollisionTable.h>

#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/CollisionData.h>

#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/Graphics/Components/Transform.h>

using namespace vlx;

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
CollisionTable::Matrix CollisionTable::table =
{
	&CollisionTable::CircleToCircle,	&CollisionTable::CircleToBox,		&CollisionTable::CircleToPoint,		&CollisionTable::CircleToConvex,
	&CollisionTable::BoxToCircle,		&CollisionTable::BoxToBox,			&CollisionTable::BoxToPoint,		&CollisionTable::BoxToConvex,
	&CollisionTable::PointToCircle,		&CollisionTable::PointToBox,		&CollisionTable::PointToPoint,		&CollisionTable::PointToConvex,
	&CollisionTable::ConvexToCircle,	&CollisionTable::ConvexToBox,		&CollisionTable::ConvexToPoint,		&CollisionTable::ConvexToConvex
};

void CollisionTable::Collide(CollisionData& collision, 
	const Shape& s1, typename Shape::Type st1,
	const Shape& s2, typename Shape::Type st2)
{
	table[st2 + int(st1) * Shape::Type::Count](collision, s1, s2);
}

void CollisionTable::CircleToCircle(CollisionData& collision, const Shape& s1,  const Shape& s2)
{
	const Circle& c1 = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Circle& c2 = reinterpret_cast<const Circle&>(s2);

	Vector2f normal = Vector2f::Direction(s1.GetCenter(), s2.GetCenter());

	const float dist_sqr = normal.LengthSq();
	const float radius = c1.GetRadius() + c2.GetRadius();

	if (dist_sqr >= radius * radius)
		return;

	collision.contact_count = 1;

	if (dist_sqr == 0.0f)
	{
		collision.penetration	= c1.GetRadius();
		collision.normal		= Vector2f::UnitX;
		collision.contacts[0]	= s1.GetCenter();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	collision.penetration	= radius - distance;
	collision.normal		= normal / distance;
	collision.contacts[0]	= collision.normal * c1.GetRadius() + s1.GetCenter();
}
void CollisionTable::CircleToBox(CollisionData& collision, const Shape& s1, const Shape& s2)
{
	const Circle& c1 = reinterpret_cast<const Circle&>(s1);
	const Box& b2 = reinterpret_cast<const Box&>(s2);

	const Vector2f half_extends(
		b2.GetWidth() / 2.0f,
		b2.GetHeight() / 2.0f);

	Vector2f n = s2.GetInverseTransform() * s1.GetCenter();
	Vector2f clamped = n.Clamp(-half_extends, half_extends);

	bool inside = false;

	if (n == clamped)
	{
		inside = true;
		if (std::abs(n.x) > std::abs(n.y))
			clamped.x = (clamped.x > 0.0f) ? half_extends.x : -half_extends.x;
		else
			clamped.y = (clamped.y > 0.0f) ? half_extends.y : -half_extends.y;
	}

	Vector2f point = s2.GetTransform() * clamped;
	Vector2f normal = Vector2f::Direction(s1.GetCenter(), point);
	float dist = normal.LengthSq();

	if ((dist > c1.GetRadiusSqr()) && !inside)
		return;

	dist = std::sqrt(dist);
	n = (normal / dist);

	collision.contact_count = 1;
	collision.penetration	= c1.GetRadius() - dist;
	collision.normal		= (inside ? -n : n);
	collision.contacts[0]	= point;
}
void CollisionTable::CircleToPoint(CollisionData&, const Shape&, const Shape&)
{

}
void CollisionTable::CircleToConvex(CollisionData& collision, const Shape& s1, const Shape& s2)
{

}

void CollisionTable::BoxToCircle(CollisionData& collision, const Shape& s1, const Shape& s2)
{
	CircleToBox(collision, s2, s1);
	collision.normal = -collision.normal; // flip normal
}
void CollisionTable::BoxToBox(CollisionData& collision, const Shape& s1, const Shape& s2)
{
	const Box& b1 = reinterpret_cast<const Box&>(s1);
	const Box& b2 = reinterpret_cast<const Box&>(s2);


}
void CollisionTable::BoxToPoint(CollisionData&, const Shape&,const Shape&)
{
}
void CollisionTable::BoxToConvex(CollisionData& collision, const Shape& s1, const Shape& s2)
{

}

void CollisionTable::PointToCircle(CollisionData&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToBox(CollisionData&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToPoint(CollisionData&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToConvex(CollisionData&, const Shape&, const Shape&)
{
}

void CollisionTable::ConvexToCircle(CollisionData& collision, const Shape& s1, const Shape& s2)
{

}
void CollisionTable::ConvexToBox(CollisionData& collision, const Shape& s1, const Shape& s2)
{

}
void CollisionTable::ConvexToPoint(CollisionData&, const Shape&, const Shape&)
{
}
void CollisionTable::ConvexToConvex(CollisionData& collision, const Shape& s1, const Shape& s2)
{

}
