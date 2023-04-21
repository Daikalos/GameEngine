#include <Velox/Physics/Shapes/Point.h>

using namespace vlx;

void Point::AdjustBody(PhysicsBody& body) const
{

}

Vector2f Point::ComputeCenter(const Vector2f& position) const
{
    return position + m_offset;
}

RectFloat Point::ComputeAABB(const GlobalTransform& transform) const
{
    return RectFloat();
}