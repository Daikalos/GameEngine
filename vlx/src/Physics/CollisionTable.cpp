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

void CollisionTable::CircleToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Circle& B = reinterpret_cast<const Circle&>(s2);

	Vector2f normal = Vector2f::Direction(A.GetCenter(), B.GetCenter());

	const float dist_sqr = normal.LengthSq();
	const float radius = A.GetRadius() + B.GetRadius();

	if (dist_sqr >= radius * radius)
		return;

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	if (dist_sqr == 0.0f)
	{
		contact.penetration = A.GetRadius();
		contact.normal		= Vector2f::UnitX;
		contact.position	= A.GetCenter();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	contact.penetration	= radius - distance;
	contact.normal		= normal / distance;
	contact.position	= contact.normal * A.GetRadius() + A.GetCenter();
}
void CollisionTable::CircleToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);
	
	const Vector2f half_extends(
		B.GetWidth() / 2.0f, B.GetHeight() / 2.0f);

	Vector2f a_center = A.GetCenter();
	Vector2f b_center = B.GetCenter();

	Vector2f n = B.GetOrientation().GetTranspose() * (a_center - b_center); // transform circle to box model space
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

	Vector2f point = b_center + s2.GetOrientation() * clamped;
	Vector2f normal = Vector2f::Direction(a_center, point);
	float dist = normal.LengthSq();

	if ((dist > A.GetRadiusSqr()) && !inside)
		return;

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	if (dist == 0.0f)
	{
		contact.penetration = A.GetRadius();
		contact.normal		= Vector2f::UnitX;
		contact.position	= point;

		return;
	}

	dist = std::sqrt(dist);
	n = (normal / dist);

	contact.penetration	= A.GetRadius() - dist;
	contact.normal		= (inside ? -n : n);
	contact.position	= point;
}
void CollisionTable::CircleToPoint(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Point& B = reinterpret_cast<const Point&>(s2);

	Vector2f normal = Vector2f::Direction(A.GetCenter(), B.GetCenter());
	const float dist_sqr = normal.LengthSq();

	if (dist_sqr >= A.GetRadiusSqr())
		return;

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	if (dist_sqr == 0.0f)
	{
		contact.penetration = A.GetRadius();
		contact.normal		= Vector2f::UnitX;
		contact.position	= A.GetCenter();

		return;
	}

	const float distance = std::sqrt(dist_sqr);

	contact.penetration = A.GetRadius() - distance;
	contact.normal = normal / distance;
	contact.position = A.GetCenter() + contact.normal * A.GetRadius();
}
void CollisionTable::CircleToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{

}

void CollisionTable::BoxToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToBox(arbiter, s2, s1);
	arbiter.contacts[0].normal *= -1;
}
void CollisionTable::BoxToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);

	enum Axis
	{
		FACE_A_X,
		FACE_A_Y,
		FACE_B_X,
		FACE_B_Y
	};

	const Vector2f a_half(A.GetWidth() / 2.0f, A.GetHeight() / 2.0f);
	const Vector2f b_half(B.GetWidth() / 2.0f, B.GetHeight() / 2.0f);

	Vector2f ap = A.GetCenter();
	Vector2f bp = B.GetCenter();

	Vector2f dp = Vector2f::Direction(ap, bp);

	const Mat2f& ar = A.GetOrientation();
	const Mat2f& br = B.GetOrientation();

	Mat2f at = ar.GetTranspose();

	Mat2f c	= at * br;
	Mat2f c_abs	= c.GetAbs();

	Vector2f da	= at * dp;

	Vector2f face_a = da.Abs() - a_half - c_abs * b_half;
	if (face_a.x > 0.0f || face_a.y > 0.0f)
		return;

	Mat2f bt = br.GetTranspose();
	Mat2f ct_abs = c_abs.GetTranspose();

	Vector2f db	= bt * dp;

	Vector2f face_b = db.Abs() - ct_abs * a_half - b_half;
	if (face_b.x > 0.0f || face_b.y > 0.0f)
		return;

	Axis axis	= FACE_A_X;
	float sep	= face_a.x;

	if (BoxBiasGreaterThan(face_a.y, sep, a_half.y))
	{
		axis	= FACE_A_Y;
		sep		= face_a.y;
	}

	if (BoxBiasGreaterThan(face_b.x, sep, b_half.x))
	{
		axis	= FACE_B_X;
		sep		= face_b.x;
	}

	if (BoxBiasGreaterThan(face_b.y, sep, b_half.y))
	{
		axis	= FACE_B_Y;
		sep		= face_b.y;
	}

	Face		incident_face;
	Vector2f	face_normal;
	Vector2f	side_normal;
	float		front		= 0.0f;
	float		neg_side	= 0.0f;
	float		pos_side	= 0.0f;
	bool		flip		= false;

	switch (axis)
	{
	case FACE_A_X:
		{
			face_normal = da.x > 0.0f ? ar.GetAxisX() : -ar.GetAxisX();
			side_normal = face_normal.Perpendicular();
			front		= ap.Dot(face_normal) + a_half.x;
			float side	= ap.Dot(side_normal);
			neg_side	= -side + a_half.y;
			pos_side	=  side + a_half.y;
			flip		= false;

			incident_face = BoxFindIncidentFace(br, bt, b_half, bp, face_normal);
		}
		break;
	case FACE_A_Y:
		{
			face_normal = da.y > 0.0f ? ar.GetAxisY() : -ar.GetAxisY();
			side_normal = face_normal.Perpendicular();
			front		= ap.Dot(face_normal) + a_half.y;
			float side	= ap.Dot(side_normal);
			neg_side	= -side + a_half.x;
			pos_side	=  side + a_half.x;
			flip		= false;

			incident_face = BoxFindIncidentFace(br, bt, b_half, bp, face_normal);
		}
		break;
	case FACE_B_X:
		{
			face_normal = -(db.x > 0.0f ? br.GetAxisX() : -br.GetAxisX());
			side_normal = face_normal.Perpendicular();
			front		= bp.Dot(face_normal) + b_half.x;
			float side	= bp.Dot(side_normal);
			neg_side	= -side + b_half.y;
			pos_side	=  side + b_half.y;
			flip		= true;

			incident_face = BoxFindIncidentFace(ar, at, a_half, ap, face_normal);
		}
		break;
	case FACE_B_Y:
		{
			face_normal = -(db.y > 0.0f ? br.GetAxisY() : -br.GetAxisY());
			side_normal = face_normal.Perpendicular();
			front		= bp.Dot(face_normal) + b_half.y;
			float side	= bp.Dot(side_normal);
			neg_side	= -side + b_half.x;
			pos_side	=  side + b_half.x;
			flip		= true;

			incident_face = BoxFindIncidentFace(ar, at, a_half, ap, face_normal);
		}
		break;
	}

	if (Clip(incident_face, -side_normal, neg_side) < 2)
		return;

	if (Clip(incident_face, side_normal, pos_side) < 2)
		return;

	uint32_t cp = 0;
	for (int i = 0; i < 2; ++i)
	{
		float separation = face_normal.Dot(incident_face[i]) - front;
		if (separation <= 0.0f)
		{
			CollisionContact& contact = arbiter.contacts[cp];

			contact.penetration = -separation;
			contact.normal = flip ? -face_normal : face_normal;
			contact.position = incident_face[i] - separation * face_normal;

			++cp;
		}
	}

	arbiter.contacts_count = cp;
}
void CollisionTable::BoxToPoint(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Point& B = reinterpret_cast<const Point&>(s2);

	const Vector2f half_extends(
		A.GetWidth() / 2.0f, A.GetHeight() / 2.0f);

	Vector2f a_center = A.GetCenter();
	Vector2f b_center = B.GetCenter();

	Vector2f n = A.GetOrientation().GetTranspose() * Vector2f::Direction(a_center, b_center); // transform circle to box model space
	Vector2f clamped = n.Clamp(-half_extends, half_extends); // then clamp vector to get the point on the boundary

	bool inside = false;

	if (n == clamped)
	{
		n /= half_extends;

		inside = true;
		if (std::abs(n.x) > std::abs(n.y))
			clamped.x = (clamped.x > 0.0f) ? half_extends.x : -half_extends.x;
		else
			clamped.y = (clamped.y > 0.0f) ? half_extends.y : -half_extends.y;
	}

	if (!inside)
		return;

	Vector2f point = a_center + A.GetOrientation() * clamped;
	Vector2f normal = Vector2f::Direction(b_center, point);
	float dist = normal.Length();

	CollisionContact& contact = arbiter.contacts[0];
	arbiter.contacts_count = 1;

	if (dist == 0.0f)
	{
		contact.penetration = 0.0f;
		contact.normal		= Vector2f::UnitX;
		contact.position	= point;

		return;
	}

	contact.penetration = dist;
	contact.normal		= normal / dist;
	contact.position	= point;

}
void CollisionTable::BoxToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Polygon& B = reinterpret_cast<const Polygon&>(s2);


}

void CollisionTable::PointToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToPoint(arbiter, s2, s1);
	arbiter.contacts[0].normal *= -1;
}
void CollisionTable::PointToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	BoxToPoint(arbiter, s2, s1);
	arbiter.contacts[0].normal *= -1;
}
void CollisionTable::PointToPoint(CollisionArbiter& arbiter, const Shape&, const Shape&)
{
	// points cannot collide with each other since they are infinitely small points
	arbiter.contacts_count = 0;
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
	const Polygon& A = reinterpret_cast<const Polygon&>(s1);
	const Polygon& B = reinterpret_cast<const Polygon&>(s2);

	auto [penetration_a, face_a] = FindAxisLeastPenetration(
		A, A.GetVertices(), A.GetNormals(),
		B, B.GetVertices(), B.GetNormals());

	if (penetration_a >= 0.0f)
		return;

	auto [penetration_b, face_b] = FindAxisLeastPenetration(
		B, B.GetVertices(), B.GetNormals(),
		A, A.GetVertices(), A.GetNormals());

	if (penetration_b >= 0.0f)
		return;

	const Polygon* ref_poly = nullptr;
	const Polygon* inc_poly = nullptr;
	uint32_t ref_idx = 0;
	bool flip = false;

	if (BiasGreaterThan(penetration_a, penetration_b))
	{
		ref_poly = &A;
		inc_poly = &B;
		ref_idx = face_a;
		flip = false;
	}
	else
	{
		ref_poly = &B;
		inc_poly = &A;
		ref_idx = face_b;
		flip = true;
	}

	Vector2f ref_pos = ref_poly->GetCenter();

	Face incident_face = FindIncidentFace(
		*inc_poly, inc_poly->GetVertices(), inc_poly->GetNormals(),
		*ref_poly, ref_poly->GetNormals()[ref_idx]);

	Vector2f v1 = ref_pos + ref_poly->GetOrientation() * ref_poly->GetVertices()[ref_idx];
	ref_idx = ((ref_idx + 1) == (uint32_t)ref_poly->GetVertices().size()) ? 0 : ref_idx + 1;
	Vector2f v2 = ref_pos + ref_poly->GetOrientation() * ref_poly->GetVertices()[ref_idx];

	Vector2f side_normal = Vector2f::Direction(v1, v2).Normalize();

	float neg_side = -side_normal.Dot(v1);
	float pos_side = side_normal.Dot(v2);

	if (Clip(incident_face, -side_normal, neg_side) < 2)
		return;

	if (Clip(incident_face, side_normal, pos_side) < 2)
		return;

	Vector2f face_normal = side_normal.Orthogonal();
	float front = face_normal.Dot(v1);

	uint32_t cp = 0;
	for (int i = 0; i < 2; ++i)
	{
		float separation = face_normal.Dot(incident_face[i]) - front;
		if (separation <= 0.0f)
		{
			CollisionContact& contact = arbiter.contacts[cp];

			contact.penetration = -separation;
			contact.normal = flip ? -face_normal : face_normal;
			contact.position = incident_face[i];

			++cp;
		}
	}

	arbiter.contacts_count = cp;
}

auto CollisionTable::BoxFindIncidentFace(const Mat2f& rot, const Mat2f& rot_tsp, 
	const Vector2f& half, const Vector2f& pos, const Vector2f& normal) -> Face
{
	Face face;

	Vector2f n = -(rot_tsp * normal);
	Vector2f n_abs = n.Abs();

	if (n_abs.x > n_abs.y)
	{
		if (n.x >= 0.0f)
		{
			face[0] = {  half.x, -half.y };
			face[1] = {  half.x,  half.y };
		}
		else
		{
			face[0] = { -half.x,  half.y };
			face[1] = { -half.x, -half.y };
		}
	}
	else
	{
		if (n.y >= 0.0f)
		{
			face[0] = {  half.x, half.y };
			face[1] = { -half.x, half.y };
		}
		else
		{
			face[0] = { -half.x, -half.y };
			face[1] = {  half.x, -half.y };
		}
	}

	face[0] = pos + rot * face[0];
	face[1] = pos + rot * face[1];

	return face;
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
	const Shape& s1, VectorSpan v1, VectorSpan n1,
	const Shape& s2, VectorSpan v2, VectorSpan n2)
{
	float best_distance = -FLT_MAX;
	uint32_t best_index = 0;

	Mat2f s2t = s2.GetOrientation().GetTranspose();
	Mat2f wt2 = s2t * s1.GetOrientation(); // combined matrix transforming to world -> model space of s2

	Vector2f dir = Vector2f::Direction(s2.GetCenter(), s1.GetCenter());

	for (uint32_t i = 0; i < v1.size(); ++i)
	{
		Vector2f normal = wt2 * n1[i]; // normal to world space -> model space of s2
		Vector2f support = GetSupport(v2, -normal); // support from s2 along -normal
		
		Vector2f vertex = s2t * (s1.GetOrientation() * v1[i] + dir); // model space of s1 -> model space of s2

		float d = normal.Dot(support - vertex); // penetration distance in s2 space

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
	uint32_t face_index = 0;

	for (uint32_t i = 0; i < inc_normals.size(); ++i) // find 
	{
		float dot = normal.Dot(inc_normals[i]);
		if (dot < min_dot)
		{
			min_dot = dot;
			face_index = i;
		}
	}

	Face face;
	Vector2f pos = inc.GetCenter();

	face[0] = pos + inc.GetOrientation() * inc_vertices[face_index];
	face_index = ((face_index + 1) >= (uint32_t)inc_normals.size()) ? 0 : face_index + 1;
	face[1] = pos + inc.GetOrientation() * inc_vertices[face_index];

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
