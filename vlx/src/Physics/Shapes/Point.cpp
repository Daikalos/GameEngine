#include <Velox/Physics/Shapes/Point.h>

using namespace vlx;

void Point::InitializeImpl(PhysicsBody& body) const
{

}
void Point::UpdateAABBImpl(const Transform& transform)
{
    m_aabb = transform.GetTransform().TransformRect(RectFloat(-0.5f, -0.5f, 1.0f, 1.0f));
}