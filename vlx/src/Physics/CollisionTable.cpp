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

void CollisionTable::Collide(CollisionData& collision, CollisionObject& a, CollisionObject& b)
{
	Shape& s1 = *a.shape;
	Shape& s2 = *b.shape;

	Transform& t1 = *a.transform;
	Transform& t2 = *b.transform;

	const std::uint32_t ci = s2.GetType() + s1.GetType() * Shape::Type::Count;

	table[ci](collision, s1, t1, s2, t2);
}

void CollisionTable::CircleToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	Circle& c1 = reinterpret_cast<Circle&>(s1); // cast is assumed safe in this kind of context
	Circle& c2 = reinterpret_cast<Circle&>(s2);

	Vector2f normal = Vector2f::Direction(t1.GetPosition(), t2.GetPosition());

	const float dist_sqr = normal.LengthSq();
	const float radius = c1.GetRadius() + c2.GetRadius();

	if (dist_sqr >= radius * radius)
		return;

	collision.contact_count = 1;

	if (dist_sqr == 0.0f)
	{
		collision.penetration	= c1.GetRadius();
		collision.normal		= Vector2f::UnitX;
		collision.contacts[0]	= t1.GetPosition();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	collision.penetration	= radius - distance;
	collision.normal		= normal / distance;
	collision.contacts[0]	= collision.normal * c1.GetRadius() + t1.GetPosition();
}
void CollisionTable::CircleToBox(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	Circle& c1 = reinterpret_cast<Circle&>(s1);
	Box& a2 = reinterpret_cast<Box&>(s2);

	const Vector2f half_extends(a2.GetWidth() / 2.0f, a2.GetHeight() / 2.0f);

	sf::Transform box_transform = t2.GetTransform(); // weird that I need to do this, will maybe, look for solution
	box_transform.translate(half_extends); // translate by extends to position correctly

	Vector2f n = box_transform.getInverse().transformPoint(t1.GetPosition());
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

	Vector2f point = box_transform.transformPoint(clamped);
	Vector2f normal = Vector2f::Direction(t1.GetPosition(), point);
	float dist = normal.LengthSq();

	if ((dist > c1.GetRadiusSqr()) && !inside)
		return;

	dist = std::sqrt(dist);
	n = (normal / dist);

	collision.contact_count = 1;
	collision.penetration	= c1.GetRadius() - dist;
	collision.normal		= (inside ? -n : n);
	collision.contacts[0]	= point; // TODO: FIX
}
void CollisionTable::CircleToPoint(CollisionData&, Shape&, Transform&, Shape&, Transform&)
{

}
void CollisionTable::CircleToConvex(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{

}

void CollisionTable::BoxToCircle(CollisionData& collision, Shape& s1, Transform& t1, Shape& s2, Transform& t2)
{
	CircleToBox(collision, s2, t2, s1, t1);
	collision.normal = -collision.normal;
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
