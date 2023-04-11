#include <Velox/Physics/Shapes/Box.h>

using namespace vlx;

void Box::InitializeImpl(PhysicsBody& body) const
{
	body.SetMass(GetWidth() * GetHeight() * body.GetDensity());
	body.SetInertia((1.0f / 12.0f) * body.GetMass() * (au::Sqr(GetWidth()) + au::Sqr(GetHeight())));
}

void Box::UpdateAABBImpl(const Transform& transform)
{
	m_aabb = transform.GetTransform().TransformRect(GetBox());
}

void Box::UpdateTransformImpl(const Transform& transform)
{
	m_transform = transform.GetTransform();
	m_transform.translate(GetBox().Size() / 2.0f);

	m_update_inverse = true;
}