#include <Velox/Physics/Shapes/Box.h>

using namespace vlx;

auto Box::GetTypePmr() const noexcept -> Type
{
	return GetType();
}

void Box::AdjustBody(PhysicsBody& body) const
{
	body.SetMass(GetWidth() * GetHeight() * body.GetDensity());
	body.SetInertia((1.0f / 12.0f) * body.GetMass() * (au::Sqr(GetWidth()) + au::Sqr(GetHeight())));
}

RectFloat Box::ComputeAABB(const Mat4f& matrix) const
{
	return matrix.TransformRect(GetBox());
}