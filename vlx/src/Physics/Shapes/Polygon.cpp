#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

void Polygon::InitializeImpl(PhysicsBody& body) const noexcept
{

}

void Polygon::UpdateAABBImpl(const Transform& transform)
{

}

void Polygon::UpdateTransformImpl(const Transform& transform)
{
    m_transform.Set(transform.GetRotation());
    m_update_inverse = true;
}
