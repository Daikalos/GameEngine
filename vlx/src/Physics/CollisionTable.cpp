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
	const Circle& circle = reinterpret_cast<const Circle&>(s1);
	const Box& box = reinterpret_cast<const Box&>(s2);
	
	const Vector2f half_extends(
		box.GetWidth() / 2.0f,
		box.GetHeight() / 2.0f);

	Vector2f s1_center = circle.GetCenter();
	Vector2f s2_center = box.GetCenter();

	Vector2f n = box.GetOrientation().GetTranspose() * (s1_center - s2_center); // transform circle to box model space
	Vector2f clamped = n.Clamp(-half_extends, half_extends); // then clamp vector to get the point on the boundary

	bool inside = false;

	if (n == clamped)
	{
		inside = true;
		if (std::abs(n.x) > std::abs(n.y))
			clamped.x = (clamped.x > 0.0f) ? half_extends.x : -half_extends.x;
		else
			clamped.y = (clamped.y > 0.0f) ? half_extends.y : -half_extends.y;
	}

	Vector2f point = s2_center + s2.GetOrientation() * clamped;
	Vector2f normal = Vector2f::Direction(s1_center, point);
	float dist = normal.LengthSq();

	if ((dist > circle.GetRadiusSqr()) && !inside)
		return;

	dist = std::sqrt(dist);
	n = (normal / dist);

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	contact.penetration	= circle.GetRadius() - dist;
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

	arbiter.contacts_count = 0;

	auto [penetration_a, face_a] = FindAxisLeastPenetration(
		b1, b1.GetVertices(), Box::NORMALS, 
		b2, b2.GetVertices(), Box::NORMALS);

	if (penetration_a >= 0.0f)
		return;

	auto [penetration_b, face_b] = FindAxisLeastPenetration(
		b2, b2.GetVertices(), Box::NORMALS,
		b1, b1.GetVertices(), Box::NORMALS);

	if (penetration_b >= 0.0f)
		return;

	uint32_t ref_idx = 0;
	bool flip = false;

	const Box* ref_box = nullptr;
	const Box* inc_box = nullptr;

	if (ma::BiasGreaterThan(penetration_a, penetration_b))
	{
		ref_box = &b1;
		inc_box = &b2;
		ref_idx = face_a;
		flip = false;
	}
	else
	{
		ref_box = &b2;
		inc_box = &b1;
		ref_idx = face_b;
		flip = true;
	}

	Face incident_face = FindIncidentFace(
		*inc_box, inc_box->GetVertices(), Box::NORMALS, 
		*ref_box, Box::NORMALS[ref_idx]);

	Vector2f v1 = ref_box->GetCenter() + ref_box->GetOrientation() * ref_box->GetVertices()[ref_idx];
	ref_idx = ((ref_idx + 1) == (uint32_t)ref_box->GetVertices().size()) ? 0 : ref_idx + 1;
	Vector2f v2 = ref_box->GetCenter() + ref_box->GetOrientation() * ref_box->GetVertices()[ref_idx];

	Vector2f side_normal = Vector2f::Direction(v1, v2).Normalize();

	float neg_side = -side_normal.Dot(v1);
	float pos_side =  side_normal.Dot(v2);

	if (Clip(incident_face, -side_normal, neg_side) < 2)
		return;

	if (Clip(incident_face,  side_normal, pos_side) < 2)
		return;

	Vector2f ref_face_normal = side_normal.Orthogonal();
	float ref_c = ref_face_normal.Dot(v1);

	uint32_t cp = 0;
	for (int i = 0; i < 2; ++i)
	{
		float separation = ref_face_normal.Dot(incident_face[i]) - ref_c;
		if (separation <= 0.0f)
		{
			CollisionContact& contact = arbiter.contacts[cp];
			contact.penetration = -separation;
			contact.normal		= flip ? -ref_face_normal : ref_face_normal;
			contact.position	= incident_face[i];

			++cp;
		}
	}

	arbiter.contacts_count = cp;
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

Vector2f CollisionTable::GetSupport(VectorSpan vertices, const Vector2f& dir)
{
	float best_projection = -FLT_MAX;
	uint32_t best_index = 0;

	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		const float projection = vertices[i].Dot(dir);
		if (projection > best_projection)
		{
			best_projection = projection;
			best_index = i;
		}
	}

	return vertices[best_index];
}

std::tuple<float, uint32_t> CollisionTable::FindAxisLeastPenetration(
	const Shape& s0, VectorSpan v0, VectorSpan n0,
	const Shape& s1, VectorSpan v1, VectorSpan n1)
{
	float best_distance = -FLT_MAX;
	uint32_t best_index = 0;

	for (uint32_t i = 0; i < v0.size(); ++i)
	{
		Mat2f s1t = s1.GetOrientation().GetTranspose();

		Vector2f normal = s1t * (s0.GetOrientation() * n0[i]); // to world space -> model space of s1
		Vector2f support = GetSupport(v1, -normal); // support from s1 along -normal
		
		Vector2f vertex = s1t * (s0.GetOrientation() * v0[i] + 
			(s0.GetCenter() - s1.GetCenter())); // model space of s0 -> model space of s1

		float d = normal.Dot(support - vertex); // penetration distance in s1 space

		if (d > best_distance)
		{
			best_distance = d;
			best_index = i;
		}
	}

	return { best_distance, best_index };
}

auto CollisionTable::FindIncidentFace(
	const Shape& inc, VectorSpan inc_vertices, VectorSpan inc_normals,
	const Shape& ref, const Vector2f& ref_normal) -> Face
{
	// calculate normal in the incident's space
	Vector2f normal = inc.GetOrientation().GetTranspose() * 
		(ref.GetOrientation() * ref_normal); // world space -> incident model space

	float min_dot = FLT_MAX;
	uint32_t incident_face = 0;

	for (std::uint32_t i = 0; i < inc_normals.size(); ++i) // find 
	{
		float dot = normal.Dot(inc_normals[i]);
		if (dot < min_dot)
		{
			min_dot = dot;
			incident_face = i;
		}
	}

	Face face;
	Vector2f pos = inc.GetCenter();

	// assign the faces
	face[0] = pos + inc.GetOrientation() * inc_vertices[incident_face];
	incident_face = ((incident_face + 1) >= (uint32_t)inc_normals.size()) ? 0 : incident_face + 1;
	face[1] = pos + inc.GetOrientation() * inc_vertices[incident_face];

	return face;
}

int CollisionTable::Clip(Face& face, const Vector2f& normal, float offset)
{
	Face out = face;
	int  op  = 0; // num of out points

	float d1 = normal.Dot(face[0]) - offset;
	float d2 = normal.Dot(face[1]) - offset;

	// if negative they are behind the plane
	if (d1 <= 0.0f) out[op++] = face[0];
	if (d2 <= 0.0f) out[op++] = face[1];

	// if they are on different sides
	if (d1 * d2 < 0.0f)
	{
		assert(op < 2);

		float alpha = d1 / (d1 - d2); // intersection point of edge and plane
		out[op] = face[0] + alpha * (face[1] - face[0]); 
		++op;
	}

	face[0] = out[0];
	face[1] = out[1];

	return op;
}
