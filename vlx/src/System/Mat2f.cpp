#include <Velox/System/Mat2f.hpp>

using namespace vlx;

Mat2f::Mat2f(sf::Angle angle)
{
	Set(angle);
}

void Mat2f::Set(sf::Angle angle)
{
	const float rad = angle.asRadians();

	const float cos = std::cos(rad);
	const float sin = std::sin(rad);

	*this = Mat2f(cos, -sin, 
				  sin,  cos);
}