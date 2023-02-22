#include <Velox/Physics/Box.h>

using namespace vlx;

constexpr Shape::Type Box::GetType() const noexcept
{
	return Shape::Box;
}

void Box::Initialize(PhysicsBody& body) const
{
}

RectFloat Box::GetAABB() const
{
	return RectFloat();
}
