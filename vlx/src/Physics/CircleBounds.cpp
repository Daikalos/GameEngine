#include <Velox/Physics/CircleBounds.h>

using namespace vlx;

CollisionResult CircleBounds::Collide(const Circle& other)
{
    return CollisionResult();
}

CollisionResult CircleBounds::Collide(const Rectangle& other)
{
    return CollisionResult();
}
