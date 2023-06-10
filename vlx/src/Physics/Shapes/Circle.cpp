#include <Velox/Physics/Shapes/Circle.h>

using namespace vlx;

void Circle::AdjustBody(PhysicsBody& body) const
{
    body.SetMass(au::PI<> * m_radius_sqr * body.GetDensity());
    body.SetInertia(0.5f * body.GetMass() * m_radius_sqr);
}

RectFloat Circle::ComputeAABB(const Transform& transform) const
{
    return RectFloat(transform.GetPosition() - transform.GetOrigin(), Vector2f(m_radius * 2.0f, m_radius * 2.0f));
}