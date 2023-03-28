#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

constexpr auto Polygon::GetType() const noexcept -> Type
{
	return Type::Convex;
}

void Polygon::UpdateAABBImpl(const Transform& transform)
{

}

void Polygon::InitializeImpl(PhysicsBody& body) const noexcept
{

}
