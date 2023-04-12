#include <Velox/Physics/Shapes/Circle.h>

using namespace vlx;

void Circle::InitializeImpl(PhysicsBody& body) const
{
    body.SetMass(au::PI<> * m_radius_sqr * body.GetDensity());
    body.SetInertia(0.5f * body.GetMass() * m_radius_sqr);
}

void Circle::UpdateAABBImpl(const Transform& transform)
{
    m_aabb = transform.GetTransform().TransformRect(RectFloat(0, 0, m_radius * 2.0f, m_radius * 2.0f));
}

void Circle::UpdateTransformImpl(const Transform& transform)
{
    m_transform.Set(transform.GetRotation());
    m_update_inverse = true;
}
