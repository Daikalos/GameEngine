#include <Velox/Physics/Shapes/Point.h>

using namespace vlx;

auto Point::GetTypePmr() const noexcept -> Type
{
    return GetType();
}

void Point::AdjustBody(PhysicsBody& body) const
{

}