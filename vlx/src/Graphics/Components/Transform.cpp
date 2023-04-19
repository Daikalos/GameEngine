#include <Velox/Graphics/Components/Transform.h>

using namespace vlx;

Transform::Transform(const Vector2f& position, const Vector2f& scale, const sf::Angle& rotation)
	: m_origin(0, 0), m_position(position), m_scale(scale), m_rotation(rotation)
{

}

Transform::Transform()
	: Transform({ 0, 0 }, { 1.0f, 1.0f }, sf::radians(0.0f)) { }
Transform::Transform(const Vector2f& position, const Vector2f& scale)
	: Transform(position, scale, sf::radians(0.0f)) {}
Transform::Transform(const Vector2f& position, const sf::Angle& rotation)
	: Transform(position, { 1.0f, 1.0f }, rotation) {}
Transform::Transform(const Vector2f& position)
	: Transform(position, { 1.0f, 1.0f }, sf::radians(0.0f)) {}

const Mat4f& Transform::GetTransform() const
{
	if (m_update)
	{
		const float angle	= -m_rotation.asRadians();
		const float cosine	= std::cos(angle);
		const float sine	= std::sin(angle);
		const float sxc		= m_scale.x * cosine;
		const float syc		= m_scale.y * cosine;
		const float sxs		= m_scale.x * sine;
		const float sys		= m_scale.y * sine;
		const float tx		= m_position.x - m_origin.x * sxc - m_origin.y * sys;
		const float ty		= m_position.y + m_origin.x * sxs - m_origin.y * syc;

		m_transform = Mat4f(
			 sxc, sys, tx,
			-sxs, syc, ty,
			 0.f, 0.f, 1.f);

		m_update = false;
	}

	return m_transform;
}
const Mat4f& Transform::GetInverseTransform() const
{
	if (m_update_inverse)
	{
		m_inverse_transform = GetTransform().GetInverse();
		m_update_inverse = false;
	}

	return m_inverse_transform;
}
const Vector2f& Transform::GetOrigin() const
{
	return m_origin;
}
const Vector2f& Transform::GetPosition() const
{
	return m_position;
}
const Vector2f& Transform::GetScale() const
{
	return m_scale;
}
const sf::Angle& Transform::GetRotation() const
{
	return m_rotation;
}

void Transform::SetOrigin(const Vector2f& origin)
{
	if (origin != m_origin)
	{
		m_origin = origin;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void Transform::SetPosition(const Vector2f& position)
{
	if (position != m_position)
	{
		m_position = position;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void Transform::SetScale(const Vector2f& scale)
{
	if (scale != m_scale)
	{
		m_scale = scale;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void Transform::SetRotation(const sf::Angle angle)
{
	sf::Angle rotation = angle.wrapUnsigned();
	if (rotation != m_rotation)
	{
		m_rotation = rotation;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}

void Transform::Move(const Vector2f& move)
{
	SetPosition(GetPosition() + move);
}
void Transform::Scale(const Vector2f& factor)
{
	const Vector2f scale = GetScale();
	SetScale({ scale.x * factor.x, scale.y * factor.y });
}
void Transform::Rotate(const sf::Angle angle)
{
	SetRotation(GetRotation() + angle);
}