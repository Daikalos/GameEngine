#include <Velox/Physics/Collision/CollisionTable.h>

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

LocalManifold CollisionTable::Collide(
	const Shape& s1, const SimpleTransform& t1, typename Shape::Type st1,
	const Shape& s2, const SimpleTransform& t2, typename Shape::Type st2)
{
	return table[st2 + st1 * Shape::Type::Count](s1, t1, s2, t2);
}

LocalManifold CollisionTable::CircleToCircle(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	LocalManifold lm{};

	const Circle& A = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Circle& B = reinterpret_cast<const Circle&>(s2);

	Vector2f normal = Vector2f::Direction(t1.GetPosition(), t2.GetPosition());

	const float dist_sqr = normal.LengthSq();
	const float radius = A.GetRadius() + B.GetRadius();

	if (dist_sqr >= radius * radius)
		return lm;

	lm.type				= LocalManifold::Type::Circles;
	lm.contacts_count	= 1;

	return lm;
}
LocalManifold CollisionTable::CircleToBox(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	const Circle& A = reinterpret_cast<const Circle&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);

	return CircleToPolygon(
		t1.GetPosition(), A.GetRadius(),
		t2, s2.GetRadius(), B.GetVertices(), Box::NORMALS);
}
LocalManifold CollisionTable::CircleToPoint(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	LocalManifold lm{};

	const Circle& A = reinterpret_cast<const Circle&>(s1); // cast is assumed safe in this kind of context
	const Point& B = reinterpret_cast<const Point&>(s2);

	Vector2f normal = Vector2f::Direction(t1.GetPosition(), t2.GetPosition());

	const float dist_sqr = normal.LengthSq();
	const float radius = A.GetRadius() + B.GetRadius();

	if (dist_sqr >= radius * radius)
		return lm;

	lm.type = LocalManifold::Type::Circles;
	lm.contacts_count = 1;
}
LocalManifold CollisionTable::CircleToConvex(
	const Shape& s1, const SimpleTransform& t1,
	const Shape& s2, const SimpleTransform& t2)
{
	const Circle& A		= reinterpret_cast<const Circle&>(s1);
	const Polygon& B	= reinterpret_cast<const Polygon&>(s2);

	return CircleToPolygon(
		t1.GetPosition(), A.GetRadius(),
		t2, B.GetRadius(), B.GetVertices(), B.GetNormals());
}

LocalManifold CollisionTable::BoxToCircle(
	const Shape& s1, const SimpleTransform& t1,
	const Shape& s2, const SimpleTransform& t2)
{
	LocalManifold lm = CircleToBox(s2, t2, s1, t1);
	lm.type = LocalManifold::Type::FaceA;
	return lm;
}
LocalManifold CollisionTable::BoxToBox(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Box& B = reinterpret_cast<const Box&>(s2);

	return PolygonToPolygon(
		t1, A.GetRadius(), A.GetVertices(), Box::NORMALS,
		t2, B.GetRadius(), B.GetVertices(), Box::NORMALS);
}
LocalManifold CollisionTable::BoxToPoint(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	const Box& A = reinterpret_cast<const Box&>(s1);
	const Point& B = reinterpret_cast<const Point&>(s2);

	return {};
}
LocalManifold CollisionTable::BoxToConvex(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	const Box& A		= reinterpret_cast<const Box&>(s1);
	const Polygon& B	= reinterpret_cast<const Polygon&>(s2);

	return PolygonToPolygon(
		t1, A.GetRadius(), A.GetVertices(), Box::NORMALS,
		t2, B.GetRadius(), B.GetVertices(), B.GetNormals());
}

LocalManifold CollisionTable::PointToCircle(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	return CircleToPoint(s2, t2, s1, t1);
}
LocalManifold CollisionTable::PointToBox(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	return BoxToPoint(s2, t2, s1, t1);
}
LocalManifold CollisionTable::PointToPoint(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	// points cannot collide with each other since they are infinitely small points
	return {};
}
LocalManifold CollisionTable::PointToConvex(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	return {};
}

LocalManifold CollisionTable::ConvexToCircle(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	LocalManifold lm = CircleToConvex(s2, t2, s1, t1);
	lm.type = LocalManifold::Type::FaceA;
	return lm;
}
LocalManifold CollisionTable::ConvexToBox(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	LocalManifold lm = BoxToConvex(s2, t2, s1, t1);
	lm.type = (lm.type == LocalManifold::Type::FaceA) ? LocalManifold::Type::FaceB : LocalManifold::Type::FaceA;
	return lm;
}
LocalManifold CollisionTable::ConvexToPoint(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	return {};
}
LocalManifold CollisionTable::ConvexToConvex(
	const Shape& s1, const SimpleTransform& t1, 
	const Shape& s2, const SimpleTransform& t2)
{
	const Polygon& A = reinterpret_cast<const Polygon&>(s1);
	const Polygon& B = reinterpret_cast<const Polygon&>(s2);

	return PolygonToPolygon(
		t1, A.GetRadius(), A.GetVertices(), A.GetNormals(),
		t2, B.GetRadius(), B.GetVertices(), B.GetNormals());
}		

LocalManifold CollisionTable::CircleToPolygon(
	const Vector2f& pos1, float radius1,
	const SimpleTransform& t2, float radius2, VectorSpan vertices, VectorSpan normals)
{
	LocalManifold lm{};

	const Vector2f center = t2.Inverse(pos1); // transform circle to polygon model space

	const float radius = radius1 + radius2;

	float separation = -FLT_MAX;
	uint32 face = 0;

	for (uint32 i = 0; i < vertices.size(); ++i)
	{
		const Vector2f dir = Vector2f::Direction(vertices[i], center);
		const float s = normals[i].Dot(dir);

		if (s > radius)
			return lm;

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
		lm.type				= LocalManifold::Type::FaceB;
		lm.contacts_count	= 1;
		lm.normal			= normals[face];
		lm.point			= 0.5f * (v1 + v2);

		return lm;
	}

	const Vector2f dir1 = Vector2f::Direction(v1, center);
	const Vector2f dir2 = Vector2f::Direction(v2, center);

	const float dot1 = dir1.Dot(Vector2f::Direction(v1, v2));
	const float dot2 = dir2.Dot(Vector2f::Direction(v2, v1));

	if (dot1 <= 0.0f)
	{
		if (dir1.LengthSq() > radius * radius)
			return lm;

		lm.type				= LocalManifold::Type::FaceB;
		lm.contacts_count	= 1;
		lm.normal			= dir1.Normalize();
		lm.point			= v1;
	}
	else if (dot2 <= 0.0f)
	{
		if (dir2.LengthSq() > radius * radius)
			return lm;

		lm.type				= LocalManifold::Type::FaceB;
		lm.contacts_count	= 1;
		lm.normal			= dir2.Normalize();
		lm.point			= v2;
	}
	else
	{
		const Vector2f face_center = 0.5f * (v1 + v2);
		const Vector2f dir3 = Vector2f::Direction(face_center, center);

		if (dir3.Dot(normals[face]) > radius)
			return lm;

		lm.type				= LocalManifold::Type::FaceB;
		lm.contacts_count	= 1;
		lm.normal			= normals[face];
		lm.point			= face_center;
	}

	return lm;
}

LocalManifold CollisionTable::PolygonToPolygon(
	const SimpleTransform& t1, float radius1, VectorSpan vs1, VectorSpan ns1,
	const SimpleTransform& t2, float radius2, VectorSpan vs2, VectorSpan ns2)
{
	LocalManifold lm{};

	const float radius = radius1 + radius2;

	auto [penetration_a, face_a] = FindAxisLeastPenetration(
		t1, vs1, ns1, t2, vs2, ns2);

	if (penetration_a > radius)
		return lm;

	auto [penetration_b, face_b] = FindAxisLeastPenetration(
		t2, vs2, ns2, t1, vs1, ns1);

	if (penetration_b > radius)
		return lm;

	const SimpleTransform*	ref_transform {nullptr};
	VectorSpan				ref_vertices;
	VectorSpan				ref_normals;

	const SimpleTransform*	inc_transform {nullptr};
	VectorSpan				inc_vertices;
	VectorSpan				inc_normals;

	uint32 ref_idx = 0;

	if (BiasGreaterThan(penetration_a, penetration_b))
	{
		ref_transform	= &t1;
		ref_vertices	= vs1;
		ref_normals		= ns1;

		inc_transform	= &t2;
		inc_vertices	= vs2;
		inc_normals		= ns2;

		ref_idx = face_a;

		lm.type = LocalManifold::Type::FaceA;
	}
	else
	{
		ref_transform	= &t2;
		ref_vertices	= vs2;
		ref_normals		= ns2;

		inc_transform	= &t1;
		inc_vertices	= vs1;
		inc_normals		= ns1;

		ref_idx = face_b;

		lm.type = LocalManifold::Type::FaceB;
	}

	Face incident_face = FindIncidentFace(
		*inc_transform, inc_vertices, inc_normals,
		*ref_transform, ref_normals[ref_idx]);

	Vector2f l1 = ref_vertices[ref_idx];
	Vector2f v1 = ref_transform->Transform(l1);

	ref_idx = ((ref_idx + 1) == (uint32)ref_vertices.size()) ? 0 : ref_idx + 1;

	Vector2f l2 = ref_vertices[ref_idx];
	Vector2f v2 = ref_transform->Transform(l2);

	Vector2f side_normal = Vector2f::Direction(v1, v2).Normalize();

	float neg_side = -side_normal.Dot(v1) + radius;
	float pos_side = side_normal.Dot(v2) + radius;

	if (Clip(incident_face, -side_normal, neg_side) < 2)
		return lm;

	if (Clip(incident_face, side_normal, pos_side) < 2)
		return lm;

	lm.normal	= Vector2f::Direction(l1, l2).Normalize().Orthogonal();
	lm.point	= 0.5f * (l1 + l2);

	Vector2f face_normal = side_normal.Orthogonal();
	float front = face_normal.Dot(v1);

	uint32 cp = 0;
	for (int i = 0; i < 2; ++i)
	{
		float penetration = front - face_normal.Dot(incident_face[i]);
		if (penetration >= -radius)
		{
			typename LocalManifold::Point& contact = lm.contacts[cp++];
			contact.point = inc_transform->Inverse(incident_face[i]);
		}
	}

	lm.contacts_count = cp;

	return lm;
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
	const SimpleTransform& t1, VectorSpan vs1, VectorSpan ns1,
	const SimpleTransform& t2, VectorSpan vs2, VectorSpan ns2)
{
	float best_distance = -FLT_MAX;
	uint32 best_index = 0;

	Rot2f wt2 = t2.GetRotation().Inverse(t1.GetRotation()); // combined matrix transforming to world -> model space of s2

	Vector2f dir = Vector2f::Direction(t2.GetPosition(), t1.GetPosition());

	for (uint32 i = 0; i < vs1.size(); ++i)
	{
		Vector2f vertex = t2.GetRotation().Inverse(t1.GetRotation().Transform(vs1[i]) + dir); // model space of s1 -> model space of s2

		Vector2f normal = wt2.Transform(ns1[i]);		// normal to world space -> model space of s2
		Vector2f support = GetSupport(vs2, -normal);	// support from s2 along -normal
		
		float d = normal.Dot(support - vertex);			// penetration distance in s2 space

		if (d > best_distance)
		{
			best_distance = d;
			best_index = i;
		}
	}

	return { best_distance, best_index };
}

auto CollisionTable::FindIncidentFace(
	const SimpleTransform& t1, VectorSpan vs1, VectorSpan ns1,
	const SimpleTransform& t2, const Vector2f& n2) -> Face
{
	// calculate normal in the incident's space
	Vector2f normal = t1.GetRotation().Inverse(t2.GetRotation().Transform(n2)); // world space -> incident model space

	float min_dot = FLT_MAX;
	uint32 face_index = 0;

	for (uint32 i = 0; i < ns1.size(); ++i) // find 
	{
		float dot = normal.Dot(ns1[i]);
		if (dot < min_dot)
		{
			min_dot = dot;
			face_index = i;
		}
	}

	Face face;

	face[0] = t1.Transform(vs1[face_index]);
	face_index = (face_index + 1) == (uint32)ns1.size() ? 0 : face_index + 1;
	face[1] = t1.Transform(vs1[face_index]);

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

// POINT BOX COLLISION
// 
//const Vector2f half_extends(
//	A.GetWidth() / 2.0f, A.GetHeight() / 2.0f);

//Vector2f n = A.GetOrientation().Inverse(Vector2f::Direction(t1.GetPosition(), t2.GetPosition())); // transform circle to box model space
//Vector2f clamped = n.Clamp(-half_extends, half_extends); // then clamp vector to get the point on the boundary

//bool inside = false;

//if (n == clamped)
//{
//	n /= half_extends;

//	inside = true;
//	if (std::abs(n.x) > std::abs(n.y))
//		clamped.x = (clamped.x > 0.0f) ? half_extends.x : -half_extends.x;
//	else
//		clamped.y = (clamped.y > 0.0f) ? half_extends.y : -half_extends.y;
//}

//if (!inside)
//	return;

//Vector2f point = a_center + A.GetOrientation().Transform(clamped);
//Vector2f normal = Vector2f::Direction(b_center, point);
//float dist = normal.Length();

//if ((dist > A.GetRadiusSqr()) && !inside)
//	return;

//arbiter.manifold.type				= LocalManifold::Type::FaceB;
//arbiter.manifold.contacts_count	= 1;
//arbiter.manifold.point			= clamped;

// CIRCLE BOX COLLISION
// 
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