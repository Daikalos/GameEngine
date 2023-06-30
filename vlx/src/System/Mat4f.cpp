#include <Velox/System/Mat4f.hpp>

using namespace vlx;

sf::Angle Mat4f::GetRotation() const
{
	return sf::radians(std::atan2f(Get(1, 0), Get(1, 1)));
}

Mat4f& Mat4f::Rotate(sf::Angle angle)
{
	m_transform.rotate(angle);
	return *this;
}

Mat4f& Mat4f::Rotate(sf::Angle angle, const Vector2f& center)
{
	m_transform.rotate(angle, center);
	return *this;
}

Mat4f& Mat4f::Build(
	const Vector2f& position,
	const Vector2f& origin,
	const Vector2f& scale,
	sf::Angle rot)
{
	const float angle	= -rot.asRadians();
	const float cos		= std::cos(angle);
	const float sin		= std::sin(angle);
	const float sxc		= scale.x * cos;
	const float syc		= scale.y * cos;
	const float sxs		= scale.x * sin;
	const float sys		= scale.y * sin;
	const float tx		= position.x - origin.x * sxc - origin.y * sys;
	const float ty		= position.y + origin.x * sxs - origin.y * syc;

	m_transform = Mat4f( sxc, sys, tx,
						-sxs, syc, ty,
						 0.f, 0.f, 1.f);

	return *this;
}

Mat4f& Mat4f::Build(const Vector2f& position, const Vector2f& scale, sf::Angle rot)
{
	const float angle	= -rot.asRadians();
	const float cos		= std::cos(angle);
	const float sin		= std::sin(angle);
	const float sxc		= scale.x * cos;
	const float syc		= scale.y * cos;
	const float sxs		= scale.x * sin;
	const float sys		= scale.y * sin;
	const float tx		= position.x;
	const float ty		= position.y;

	m_transform = Mat4f( sxc, sys, tx,
						-sxs, syc, ty,
						 0.f, 0.f, 1.f);

	return *this;
}

Mat4f& Mat4f::Build(const Vector2f& position, sf::Angle rot)
{
	const float angle	= -rot.asRadians();
	const float cos		= std::cos(angle);
	const float sin		= std::sin(angle);
	const float tx		= position.x;
	const float ty		= position.y;

	m_transform = Mat4f( cos, sin, tx,
						-sin, cos, ty,
						 0.f, 0.f, 1.f);

	return *this;
}
