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

	arbiter.manifold.type			= LocalManifold::Type::Circles;
	arbiter.manifold.contacts_count	= 1;
}
void CollisionTable::CircleToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);

	CircleToPolygon(arbiter, A, B, B.GetVertices(), Box::NORMALS);

	//const Circle& A = reinterpret_cast<const Circle&>(s1);
	//const Box& B	= reinterpret_cast<const Box&>(s2);

	//const float radius = A.GetRadius() + B.GetRadius();
	//
	//const Vector2f half_extends(
	//	B.GetRadius() + (B.GetWidth() / 2.0f), 
	//	B.GetRadius() + (B.GetHeight() / 2.0f));

	//Vector2f a_center = A.GetCenter();
	//Vector2f b_center = B.GetCenter();

	//Vector2f n = B.GetOrientation().Inverse(a_center - b_center); // transform circle to box model space
	//Vector2f clamped = n.Clamp(-half_extends, half_extends); // then clamp vector to get the point on the boundary

	//bool inside = false;

	//if (n == clamped)
	//{
	//	inside = true;
	//	if (std::abs(n.x) > std::abs(n.y))
	//		clamped.x = (clamped.x > 0.0f) ? half_extends.x : -half_extends.x;
	//	else
	//		clamped.y = (clamped.y > 0.0f) ? half_extends.y : -half_extends.y;
	//}

	//Vector2f point = b_center + B.GetOrientation().Transform(clamped);
	//Vector2f normal = Vector2f::Direction(a_center, point);
	//float dist = normal.LengthSq();

	//if ((dist > radius * radius) && !inside)
	//	return;

	//n = (dist > FLT_EPSILON * FLT_EPSILON) ? 
	//	-B.GetOrientation().Inverse(normal / std::sqrt(dist)) : Vector2f::UnitX;

	//arbiter.manifold.type			= LocalManifold::Type::FaceB;
	//arbiter.manifold.contacts_count = 1;
	//arbiter.manifold.point			= clamped;
	//arbiter.manifold.normal			= (inside ? -n : n);
}
void CollisionTable::CircleToPoint(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Point& B = reinterpret_cast<const Point&>(s2);

	Vector2f normal = Vector2f::Direction(A.GetCenter(), B.GetCenter());

	const float dist_sqr = normal.LengthSq();
	const float radius = A.GetRadius() + B.GetRadius();

	if (dist_sqr >= radius * radius)
		return;

	arbiter.manifold.type = LocalManifold::Type::Circles;
	arbiter.manifold.contacts_count = 1;
}
void CollisionTable::CircleToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Circle& A		= reinterpret_cast<const Circle&>(s1);
	const Polygon& B	= reinterpret_cast<const Polygon&>(s2);

	CircleToPolygon(arbiter, A, B, B.GetVertices(), B.GetNormals());
}

void CollisionTable::BoxToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToBox(arbiter, s2, s1);
	arbiter.manifold.type = LocalManifold::Type::FaceA;
}
void CollisionTable::BoxToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);

	PolygonToPolygon(arbiter,
		A, A.GetVertices(), Box::NORMALS,
		B, B.GetVertices(), Box::NORMALS);
}
void CollisionTable::BoxToPoint(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Point& B = reinterpret_cast<const Point&>(s2);

	const Vector2f half_extends(
		A.GetWidth() / 2.0f, A.GetHeight() / 2.0f);

	Vector2f a_center = A.GetCenter();
	Vector2f b_center = B.GetCenter();

	Vector2f n = A.GetOrientation().Inverse(Vector2f::Direction(a_center, b_center)); // transform circle to box model space
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

	Vector2f point = a_center + A.GetOrientation().Transform(clamped);
	Vector2f normal = Vector2f::Direction(b_center, point);
	float dist = normal.Length();

	if ((dist > A.GetRadiusSqr()) && !inside)
		return;

	arbiter.manifold.type			= LocalManifold::Type::FaceB;
	arbiter.manifold.contacts_count = 1;
	arbiter.manifold.point			= clamped;

}
void CollisionTable::BoxToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Box& A		= reinterpret_cast<const Box&>(s1);
	const Polygon& B	= reinterpret_cast<const Polygon&>(s2);

	PolygonToPolygon(arbiter,
		A, A.GetVertices(), Box::NORMALS, 
		B, B.GetVertices(), B.GetNormals());
}

void CollisionTable::PointToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToPoint(arbiter, s2, s1);
}
void CollisionTable::PointToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	BoxToPoint(arbiter, s2, s1);
}
void CollisionTable::PointToPoint(CollisionArbiter& arbiter, const Shape&, const Shape&)
{
	// points cannot collide with each other since they are infinitely small points
	arbiter.manifold.contacts_count = 0;
}
void CollisionTable::PointToConvex(CollisionArbiter&, const Shape&, const Shape&)
{

}

void CollisionTable::ConvexToCircle(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	CircleToConvex(arbiter, s2, s1);
	arbiter.manifold.type = LocalManifold::Type::FaceA;
}
void CollisionTable::ConvexToBox(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	BoxToConvex(arbiter, s2, s1);
	arbiter.manifold.type = (arbiter.manifold.type == LocalManifold::Type::FaceA) ? LocalManifold::Type::FaceB : LocalManifold::Type::FaceA;
}
void CollisionTable::ConvexToPoint(CollisionArbiter&, const Shape& s1, const Shape& s2)
{

}
void CollisionTable::ConvexToConvex(CollisionArbiter& arbiter, const Shape& s1, const Shape& s2)
{
	const Polygon& A = reinterpret_cast<const Polygon&>(s1);
	const Polygon& B = reinterpret_cast<const Polygon&>(s2);

	PolygonToPolygon(arbiter,
		A, A.GetVertices(), A.GetNormals(), 
		B, B.GetVertices(), B.GetNormals());
}

void CollisionTable::CircleToPolygon(CollisionArbiter& arbiter, const Shape& A,
	const ShapeRotatable& B, VectorSpan vertices, VectorSpan normals)
{
	const Vector2f a_center = A.GetCenter();
	const Vector2f b_center = B.GetCenter();

	const Vector2f center = B.GetOrientation().Inverse(a_center - b_center); // transform circle to polygon model space

	const float radius = A.GetRadius() + B.GetRadius();

	float separation = -FLT_MAX;
	uint32 face = 0;

	for (uint32 i = 0; i < vertices.size(); ++i)
	{
		const Vector2f dir = Vector2f::Direction(vertices[i], center);
		const float s = normals[i].Dot(dir);

		if (s > radius)
			return;

		if (s > separation)
		{
			separation = s;
			face = i;
		}
	}

	const uint32 face2 = (face + 1) == vertices.size() ? 0 : face + 1;

	const Vector2f v1 = vertices[face];
	const Vector2f v2 = vertices[face2];

	if (separation < FLT_EPSILON)
	{
		arbiter.manifold.type			= LocalManifold::Type::FaceB;
		arbiter.manifold.contacts_count	= 1;
		arbiter.manifold.normal			= normals[face];
		arbiter.manifold.point			= 0.5f * (v1 + v2);

		return;
	}

	const Vector2f dir1 = Vector2f::Direction(v1, center);
	const Vector2f dir2 = Vector2f::Direction(v2, center);

	const float dot1 = dir1.Dot(Vector2f::Direction(v1, v2));
	const float dot2 = dir2.Dot(Vector2f::Direction(v2, v1));

	if (dot1 <= 0.0f)
	{
		if (dir1.LengthSq() > radius * radius)
			return;

		arbiter.manifold.type			= LocalManifold::Type::FaceB;
		arbiter.manifold.contacts_count	= 1;
		arbiter.manifold.normal			= dir1.Normalize();
		arbiter.manifold.point			= v1;
	}
	else if (dot2 <= 0.0f)
	{
		if (dir2.LengthSq() > radius * radius)
			return;

		arbiter.manifold.type			= LocalManifold::Type::FaceB;
		arbiter.manifold.contacts_count	= 1;
		arbiter.manifold.normal			= dir2.Normalize();
		arbiter.manifold.point			= v2;
	}
	else
	{
		const Vector2f face_center = 0.5f * (v1 + v2);
		const Vector2f dir3 = Vector2f::Direction(face_center, center);

		if (dir3.Dot(normals[face]) > radius)
			return;

		arbiter.manifold.type			= LocalManifold::Type::FaceB;
		arbiter.manifold.contacts_count	= 1;
		arbiter.manifold.normal			= normals[face];
		arbiter.manifold.point			= face_center;
	}
}

void CollisionTable::PolygonToPolygon(CollisionArbiter& arbiter,
	const ShapeRotatable& A, VectorSpan vs1, VectorSpan ns1, 
	const ShapeRotatable& B, VectorSpan vs2, VectorSpan ns2)
{
	const float radius = A.GetRadius() + B.GetRadius();

	auto [penetration_a, face_a] = FindAxisLeastPenetration(A, vs1, ns1, B, vs2, ns2);

	if (penetration_a > radius)
		return;

	auto [penetration_b, face_b] = FindAxisLeastPenetration(B, vs2, ns2, A, vs1, ns1);

	if (penetration_b > radius)
		return;

	const ShapeRotatable* ref_shape = nullptr;
	VectorSpan ref_vertices;
	VectorSpan ref_normals;

	const ShapeRotatable* inc_shape = nullptr;
	VectorSpan inc_vertices;
	VectorSpan inc_normals;

	uint32 ref_idx = 0;

	if (BiasGreaterThan(penetration_a, penetration_b))
	{
		ref_shape		= &A;
		ref_vertices	= vs1;
		ref_normals		= ns1;

		inc_shape		= &B;
		inc_vertices	= vs2;
		inc_normals		= ns2;

		ref_idx = face_a;

		arbiter.manifold.type = LocalManifold::Type::FaceA;
	}
	else
	{
		ref_shape		= &B;
		ref_vertices	= vs2;
		ref_normals		= ns2;

		inc_shape		= &A;
		inc_vertices	= vs1;
		inc_normals		= ns1;

		ref_idx = face_b;

		arbiter.manifold.type = LocalManifold::Type::FaceB;
	}

	Face incident_face = FindIncidentFace(
		*inc_shape, inc_vertices, inc_normals,
		*ref_shape, ref_normals[ref_idx]);

	Vector2f l1 = ref_vertices[ref_idx];
	Vector2f v1 = ref_shape->GetCenter() + ref_shape->GetOrientation().Transform(l1);

	ref_idx = ((ref_idx + 1) == (uint32)ref_vertices.size()) ? 0 : ref_idx + 1;

	Vector2f l2 = ref_vertices[ref_idx];
	Vector2f v2 = ref_shape->GetCenter() + ref_shape->GetOrientation().Transform(l2);

	Vector2f side_normal = Vector2f::Direction(v1, v2).Normalize();

	float neg_side = -side_normal.Dot(v1) + radius;
	float pos_side = side_normal.Dot(v2) + radius;

	if (Clip(incident_face, -side_normal, neg_side) < 2)
		return;

	if (Clip(incident_face, side_normal, pos_side) < 2)
		return;

	arbiter.manifold.normal = Vector2f::Direction(l1, l2).Normalize().Orthogonal();
	arbiter.manifold.point = 0.5f * (l1 + l2);

	Vector2f face_normal = side_normal.Orthogonal();
	float front = face_normal.Dot(v1);

	uint32 cp = 0;
	for (int i = 0; i < 2; ++i)
	{
		float penetration = front - face_normal.Dot(incident_face[i]);
		if (penetration >= -radius)
		{
			typename LocalManifold::Point& contact = arbiter.manifold.contacts[cp++];
			contact.point = inc_shape->GetOrientation().Inverse(incident_face[i] - inc_shape->GetCenter());
		}
	}

	arbiter.manifold.contacts_count = cp;
}

Vector2f CollisionTable::GetSupport(VectorSpan vertices, const Vector2f& dir)
{
	float best_projection = -FLT_MAX;
	uint32 best_index = 0;

	for (uint32 i = 0; i < vertices.size(); ++i)
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

std::tuple<float, uint32> CollisionTable::FindAxisLeastPenetration(
	const ShapeRotatable& s1, VectorSpan v1, VectorSpan n1,
	const ShapeRotatable& s2, VectorSpan v2, VectorSpan n2)
{
	float best_distance = -FLT_MAX;
	uint32 best_index = 0;

	Rot2f wt2 = s2.GetOrientation().Inverse(s1.GetOrientation()); // combined matrix transforming to world -> model space of s2

	Vector2f dir = Vector2f::Direction(s2.GetCenter(), s1.GetCenter());

	for (uint32 i = 0; i < v1.size(); ++i)
	{
		Vector2f vertex = s2.GetOrientation().Inverse(s1.GetOrientation().Transform(v1[i]) + dir); // model space of s1 -> model space of s2

		Vector2f normal = wt2.Transform(n1[i]); // normal to world space -> model space of s2
		Vector2f support = GetSupport(v2, -normal); // support from s2 along -normal
		
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
	const ShapeRotatable& inc, VectorSpan inc_vertices, VectorSpan inc_normals,
	const ShapeRotatable& ref, const Vector2f& ref_normal) -> Face
{
	// calculate normal in the incident's space
	Vector2f normal = inc.GetOrientation().Inverse(
		ref.GetOrientation().Transform(ref_normal)); // world space -> incident model space

	float min_dot = FLT_MAX;
	uint32 face_index = 0;

	for (uint32 i = 0; i < inc_normals.size(); ++i) // find 
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

	face[0] = pos + inc.GetOrientation().Transform(inc_vertices[face_index]);
	face_index = (face_index + 1) == (uint32)inc_normals.size() ? 0 : face_index + 1;
	face[1] = pos + inc.GetOrientation().Transform(inc_vertices[face_index]);

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
