#include <Velox/System/Rot2f.h>

using namespace vlx;

Rot2f::Rot2f(sf::Angle angle) : sin(std::sinf(angle.asRadians())), cos(std::cosf(angle.asRadians()))
{

}

Rot2f Rot2f::Transform(const Rot2f& rhs) const
{
	Rot2f c{}; // combined

	c.sin = sin * rhs.cos + cos * rhs.sin;
	c.cos = cos * rhs.cos - sin * rhs.sin;

	return c;
}
Vector2f Rot2f::Transform(const Vector2f& rhs) const
{
	Vector2f c{}; // combined

	c.x = cos * rhs.x - sin * rhs.y;
	c.y = sin * rhs.x + cos * rhs.y;

	return c;
}

Rot2f Rot2f::Inverse(const Rot2f& rhs) const
{
	Rot2f i{}; // inversed

	i.sin = cos * rhs.sin - sin * rhs.cos;
	i.cos = cos * rhs.cos + sin * rhs.sin;

	return i;
}
Vector2f Rot2f::Inverse(const Vector2f& rhs) const
{
	Vector2f i{}; // combined

	i.x =  cos * rhs.x + sin * rhs.y;
	i.y = -sin * rhs.x + cos * rhs.y;

	return i;
}

void Rot2f::SetIdentity()
{
	sin = 0.0f;
	cos = 1.0f;
}

void Rot2f::Set(sf::Angle angle)
{
	const float rot = angle.asRadians();

	sin = std::sinf(rot);
	cos = std::cosf(rot);
}

float Rot2f::GetAngle() const
{
	return std::atan2f(sin, cos);
}

Vector2f Rot2f::GetAxisX() const
{
	return Vector2f(cos, sin);
}

Vector2f Rot2f::GetAxisY() const
{
	return Vector2f(-sin, cos);
}
