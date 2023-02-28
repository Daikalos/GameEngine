#include <Velox/Physics/Polygon.h>

using namespace vlx;

constexpr Shape::Type vlx::Polygon::GetType() const noexcept
{
	return Type::Convex;
}

void vlx::Polygon::Initialize(PhysicsBody& body) const noexcept
{
}

RectFloat vlx::Polygon::GetAABB() const
{
	return RectFloat();
}
