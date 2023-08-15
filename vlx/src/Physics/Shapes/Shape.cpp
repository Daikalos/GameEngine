#include <Velox/Physics/Shapes/Shape.h>

using namespace vlx;

float Shape::GetRadius() const noexcept		{ return m_radius; }
float Shape::GetRadiusSqr() const noexcept	{ return m_radius_sqr; }
