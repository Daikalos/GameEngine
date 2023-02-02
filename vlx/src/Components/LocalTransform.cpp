#include <Velox/Components/LocalTransform.h>

using namespace vlx;

LocalTransform::LocalTransform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation)
	: m_origin(0, 0), m_position(position), m_scale(scale), m_rotation(rotation)
{

}

LocalTransform::LocalTransform()
	: LocalTransform({ 0, 0 }, { 1.0f, 1.0f }, sf::radians(0.0f)) { }
LocalTransform::LocalTransform(const sf::Vector2f& position, const sf::Vector2f& scale)
	: LocalTransform(position, scale, sf::radians(0.0f)) {}
LocalTransform::LocalTransform(const sf::Vector2f& position, const sf::Angle& rotation)
	: LocalTransform(position, { 1.0f, 1.0f }, rotation) {}
LocalTransform::LocalTransform(const sf::Vector2f& position)
	: LocalTransform(position, { 1.0f, 1.0f }, sf::radians(0.0f)) {}

const sf::Transform& LocalTransform::GetTransform() const
{
	if (m_update)
	{
		const float angle	= -m_rotation.asRadians();
		const float cosine	= std::cosf(angle);
		const float sine	= std::sinf(angle);
		const float sxc		= m_scale.x * cosine;
		const float syc		= m_scale.y * cosine;
		const float sxs		= m_scale.x * sine;
		const float sys		= m_scale.y * sine;
		const float tx		= -m_origin.x * sxc - m_origin.y * sys + m_position.x;
		const float ty		= m_origin.x * sxs - m_origin.y * syc + m_position.y;

		m_transform = sf::Transform(
			sxc, sys, tx,
			-sxs, syc, ty,
			0.f, 0.f, 1.f);

		m_update = false;
	}

	return m_transform;
}
const sf::Transform& LocalTransform::GetInverseTransform() const
{
	if (m_update_inverse)
	{
		m_inverse_transform = GetTransform().getInverse();
		m_update_inverse = false;
	}

	return m_inverse_transform;
}
const sf::Vector2f& LocalTransform::GetOrigin() const
{
	return m_origin;
}
const sf::Vector2f& LocalTransform::GetPosition() const
{
	return m_position;
}
const sf::Vector2f& LocalTransform::GetScale() const
{
	return m_scale;
}
const sf::Angle& LocalTransform::GetRotation() const
{
	return m_rotation;
}

void LocalTransform::SetOrigin(const sf::Vector2f& origin)
{
	if (origin != m_origin)
	{
		m_origin = origin;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void LocalTransform::SetPosition(const sf::Vector2f& position)
{
	if (position != m_position)
	{
		m_position = position;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void LocalTransform::SetScale(const sf::Vector2f& scale)
{
	if (scale != m_scale)
	{
		m_scale = scale;

		m_update = true;
		m_update_inverse = true;

		m_dirty = true;
	}
}
void LocalTransform::SetRotation(const sf::Angle angle)
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

void LocalTransform::Move(const sf::Vector2f& move)
{
	SetPosition(GetPosition() + move);
}
void LocalTransform::Scale(const sf::Vector2f& factor)
{
	const sf::Vector2f scale = GetScale();
	SetScale({ scale.x * factor.x, scale.y * factor.y });
}
void LocalTransform::Rotate(const sf::Angle angle)
{
	SetRotation(GetRotation() + angle);
}