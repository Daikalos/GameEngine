#include <Velox/Physics/Shapes/Circle.h>

using namespace vlx;

void Circle::AdjustBody(PhysicsBody& body) const
{
    body.SetMass(au::PI<> * m_radius_sqr * body.GetDensity());
    body.SetInertia(0.5f * body.GetMass() * m_radius_sqr);
}

Vector2f Circle::ComputeCenter(const Vector2f& position) const
{
    return position + Vector2f(m_radius, m_radius);
}

RectFloat Circle::ComputeAABB(const GlobalTransform& transform) const
{
    return RectFloat(transform.GetPosition(), Vector2f(m_radius * 2.0f, m_radius * 2.0f));
}