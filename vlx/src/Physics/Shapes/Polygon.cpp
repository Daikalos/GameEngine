#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

constexpr Shape::Type Polygon::GetType() const noexcept
{
	return Type::Convex;
}

void Polygon::SetAABB(const Transform& transform)
{

}

void Polygon::Initialize(PhysicsBody& body) const noexcept
{

}
