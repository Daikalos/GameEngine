#include <Velox/Physics/Shapes/Point.h>

using namespace vlx;

void Point::AdjustBody(PhysicsBody& body) const
{

}

RectFloat Point::ComputeAABB(const Transform& transform) const
{
    return RectFloat();
}