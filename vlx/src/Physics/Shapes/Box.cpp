#include <Velox/Physics/Shapes/Box.h>

using namespace vlx;

void Box::AdjustBody(PhysicsBody& body) const
{
	body.SetMass(GetWidth() * GetHeight() * body.GetDensity());
	body.SetInertia((1.0f / 12.0f) * body.GetMass() * (au::Sqr(GetWidth()) + au::Sqr(GetHeight())));
}

Vector2f Box::ComputeCenter(const Vector2f& position) const
{
	return position + (GetSize() / 2.0f);
}

RectFloat Box::ComputeAABB(const GlobalTransform& transform) const
{
	return transform.GetTransform().TransformRect(GetBox());
}