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
		m_transform.Build(m_position, m_origin, m_scale, m_rotation);
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