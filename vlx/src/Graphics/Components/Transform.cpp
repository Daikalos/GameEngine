#include <Velox/Graphics/Components/Transform.h>

using namespace vlx;

const Mat4f& Transform::GetTransform() const noexcept
{
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
const Vector2f& Transform::GetPosition() const 
{
	if (m_update_position)
	{
		m_position.x = GetTransform().Get(0, 3);
		m_position.y = GetTransform().Get(1, 3);

		m_update_position = false;
	}

	return m_position;
}
const Vector2f& Transform::GetScale() const 
{
	if (m_update_scale)
	{
		m_scale.x = au::Sign(GetTransform().Get(0, 0)) * au::SqrtSqr(GetTransform().Get(0, 0), GetTransform().Get(1, 0));
		m_scale.y = au::Sign(GetTransform().Get(1, 1)) * au::SqrtSqr(GetTransform().Get(0, 1), GetTransform().Get(1, 1));

		m_update_scale = false;
	}

	return m_scale;
}
const sf::Angle& Transform::GetRotation() const 
{
	if (m_update_rotation)
	{
		m_rotation = sf::radians(std::atan2f(GetTransform().Get(1, 0), GetTransform().Get(1, 1)));

		m_update_rotation = false;
	}

	return m_rotation;
}