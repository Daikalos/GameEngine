#include <Velox/Physics/Shapes/Box.h>

using namespace vlx;

void Box::InitializeImpl(PhysicsBody& body) const
{
	body.SetMass(GetWidth() * GetHeight() * body.GetDensity());
	body.SetInertia((1.0f / 12.0f) * body.GetMass() * (au::Sqr(GetWidth()) + au::Sqr(GetHeight())));
}

void Box::UpdateAABBImpl(const GlobalTransform& transform)
{
	m_aabb = transform.GetTransform().TransformRect(GetBox());
}