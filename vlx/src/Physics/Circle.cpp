#include <Velox/Physics/Circle.h>

using namespace vlx;

constexpr Shape::Type Circle::GetType() const noexcept
{
	return Shape::Circle;
}
