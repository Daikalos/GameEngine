#include <Velox/Physics/Shapes/Circle.h>

using namespace vlx;

Circle::Circle(const float radius) : m_radius(radius), m_radius_sqr(au::Pow(radius)) {}

constexpr Shape::Type Circle::GetType() const noexcept      { return Type::Circle; }
constexpr float Circle::GetRadius() const noexcept          { return m_radius; }
constexpr float vlx::Circle::GetRadiusSqr() const noexcept  { return m_radius_sqr; }

constexpr void Circle::SetRadius(float radius)
{
    m_radius = radius;
    m_radius_sqr = au::Pow(radius);
}

void Circle::Initialize(PhysicsBody& body) const
{
    body.SetMass(au::PI<> * m_radius_sqr * body.GetDensity());
    body.SetInertia(0.5f * body.GetMass() * m_radius_sqr);
}

RectFloat Circle::GetAABB() const
{
    return RectFloat(0, 0, m_radius * 2.0f, m_radius * 2.0f);
}
