#include <Velox/Physics/Shapes/Point.h>

using namespace vlx;

void Point::InitializeImpl(PhysicsBody& body) const
{

}
void Point::UpdateAABBImpl(const Transform& transform)
{
    m_aabb = RectFloat(transform.GetPosition() + m_offset, {});
}