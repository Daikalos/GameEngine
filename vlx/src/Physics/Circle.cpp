#include <Velox/Physics/Circle.h>

using namespace vlx;

CollisionResult Circle::Collide(Bounds& bounds) const
{
	return bounds.Collide(*this);
}
