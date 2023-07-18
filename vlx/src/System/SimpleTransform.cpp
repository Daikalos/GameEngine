#include <Velox/System/SimpleTransform.h>

using namespace vlx;

SimpleTransform::SimpleTransform(const Vector2f& position, const Rot2f& rotation)
	: m_pos(position), m_rot(rotation)
{
}

Vector2f SimpleTransform::Transform(const Vector2f& rhs) const
{
	return m_rot.Transform(rhs) + m_pos;
}
Vector2f SimpleTransform::Inverse(const Vector2f& rhs) const
{
	return m_rot.Inverse(rhs - m_pos);
}

SimpleTransform SimpleTransform::Transform(const SimpleTransform& rhs) const
{
	SimpleTransform c;

	c.m_rot = m_rot.Transform(rhs.m_rot);
	c.m_pos = m_rot.Transform(rhs.m_pos) + m_pos;

	return c;
}
SimpleTransform SimpleTransform::Inverse(const SimpleTransform& rhs) const
{
	SimpleTransform c;

	c.m_rot = m_rot.Inverse(rhs.m_rot);
	c.m_pos = m_rot.Inverse(rhs.m_pos - m_pos);

	return c;
}

void SimpleTransform::SetIdentity()
{
	m_pos = {};
	m_rot = {};
}

void SimpleTransform::Set(const Vector2f& position, sf::Angle angle)
{
	m_pos = position;
	m_rot.Set(angle);
}

void SimpleTransform::SetPosition(const Vector2f& position)
{
	m_pos = position;
}
void SimpleTransform::SetRotation(sf::Angle angle)
{
	m_rot.Set(angle);
}

const Vector2f& SimpleTransform::GetPosition() const noexcept
{
	return m_pos;
}
const Rot2f& SimpleTransform::GetRotation() const noexcept
{
	return m_rot;
}
