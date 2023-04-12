#include <Velox/Physics/CollisionTable.h>

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

void CollisionTable::Collide(CollisionArbiter& arbiter, 
	const Shape& s1, typename Shape::Type st1,
	const Shape& s2, typename Shape::Type st2)
{
	table[st2 + int(st1) * Shape::Type::Count](arbiter, s1, s2);
}

void CollisionTable::CircleToCircle(CollisionArbiter& arbiter, const Shape& s1,  const Shape& s2)
{
	const Circle& c1 = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Circle& c2 = reinterpret_cast<const Circle&>(s2);

	Vector2f normal = Vector2f::Direction(s1.GetCenter(), s2.GetCenter());

	const float dist_sqr = normal.LengthSq();
	const float radius = c1.GetRadius() + c2.GetRadius();

	if (dist_sqr >= radius * radius)
		return;

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	if (dist_sqr == 0.0f)
	{
		contact.penetration = c1.GetRadius();
		contact.normal		= Vector2f::UnitX;
		contact.position	= s1.GetCenter();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	contact.penetration	= radius - distance;
	contact.normal		= normal / distance;
	contact.position	= contact.normal * c1.GetRadius() + s1.GetCenter();
}
void CollisionTable::CircleToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& c1 = reinterpret_cast<const Circle&>(s1);
	const Box& b2 = reinterpret_cast<const Box&>(s2);
	
	const Vector2f half_extends(
		b2.GetWidth() / 2.0f,
		b2.GetHeight() / 2.0f);

	Vector2f s1_center = s1.GetCenter();
	Vector2f s2_center = s2.GetCenter();

	Vector2f n = s2.GetTransform().GetTranspose() * (s1_center - s2_center);
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

	Vector2f point = s2_center + s2.GetTransform() * clamped;
	Vector2f normal = Vector2f::Direction(s1_center, point);
	float dist = normal.LengthSq();

	if ((dist > c1.GetRadiusSqr()) && !inside)
		return;

	dist = std::sqrt(dist);
	n = (normal / dist);

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	contact.penetration	= c1.GetRadius() - dist;
	contact.normal		= (inside ? -n : n);
	contact.position	= point;
}
void CollisionTable::CircleToPoint(CollisionArbiter&, const Shape&, const Shape&)
{

}
void CollisionTable::CircleToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}

void CollisionTable::BoxToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToBox(arbiter, s2, s1);
	arbiter.contacts[0].normal = -arbiter.contacts[0].normal;
}
void CollisionTable::BoxToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& b1 = reinterpret_cast<const Box&>(s1);
	const Box& b2 = reinterpret_cast<const Box&>(s2);


}
void CollisionTable::BoxToPoint(CollisionArbiter&, const Shape&,const Shape&)
{
}
void CollisionTable::BoxToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}

void CollisionTable::PointToCircle(CollisionArbiter&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToBox(CollisionArbiter&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToPoint(CollisionArbiter&, const Shape&, const Shape&)
{
}

void CollisionTable::PointToConvex(CollisionArbiter&, const Shape&, const Shape&)
{
}

void CollisionTable::ConvexToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}
void CollisionTable::ConvexToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}
void CollisionTable::ConvexToPoint(CollisionArbiter&, const Shape& s1, const Shape& s2)
{
}
void CollisionTable::ConvexToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}

Vector2f CollisionTable::GetSupport(std::span<const Vector2f> vertices, const Vector2f& dir)
{
	Vector2f best_vertex;
	float best_projection = -FLT_MAX;

	for (std::uint32_t i = 0; i < vertices.size(); ++i)
	{
		const Vector2f v = vertices[i];
		const float projection = v.Dot(dir);

		if (projection > best_projection)
		{
			best_vertex = v;
			best_projection;
		}
	}

	return best_vertex;
}

std::tuple<float, uint32_t> CollisionTable::FindAxisLeastPenetration(
	const Shape& s0, std::span<const Vector2f> v0, std::span<const Vector2f> n0,
	const Shape& s1, std::span<const Vector2f> v1, std::span<const Vector2f> n1)
{
	float best_distance = -FLT_MAX;
	uint32_t best_index = 0;

	for (uint32_t i = 0; i < v0.size(); ++i)
	{
		Vector2f normal = s1.GetInverseTransform() * (s0.GetTransform() * n0[i]); // TODO: fix
		Vector2f support = GetSupport(v1, -normal);
		
		Vector2f v = v0[i];

	}

	return { best_distance, best_index };
}

int CollisionTable::Clip(std::array<Vector2f, 2>& face, const Vector2f& n, float c)
{

	return 0;
}
