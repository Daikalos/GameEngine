#include <Velox/Physics/Shapes/Circle.h>

using namespace vlx;

auto Circle::GetTypePmr() const noexcept -> Type
{
    return GetType();
}

void Circle::AdjustBody(PhysicsBody& body) const
{
    body.SetMass(au::PI<> * m_radius_sqr * body.GetDensity());
    body.SetInertia(0.5f * body.GetMass() * m_radius_sqr);
}

RectFloat Circle::ComputeAABB(const Vector2f& position) const
{
    return RectFloat{position - Vector2f{m_radius, m_radius}, Vector2f{m_radius * 2.0f, m_radius * 2.0f}};
}