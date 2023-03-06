#include <Velox/Graphics/Components/Transform.h>

using namespace vlx;
using namespace vlx::mu;

const sf::Transform& Transform::GetTransform() const noexcept
{
	return m_transform;
}
const sf::Transform& Transform::GetInverseTransform() const
{
	if (m_update_inverse)
	{
		m_inverse_transform = GetTransform().getInverse();
		m_update_inverse = false;
	}

	return m_inverse_transform;
}
const Vector2f& Transform::GetPosition() const 
{
	if (m_update_position)
	{
		const float* m = GetTransform().getMatrix();

		m_position.x = MV(m, 0, 3);
		m_position.y = MV(m, 1, 3);

		m_update_position = false;
	}

	return m_position;
}
const Vector2f& Transform::GetScale() const 
{
	if (m_update_scale)
	{
		const float* m = GetTransform().getMatrix();

		m_scale.x = au::Sign(MV(m, 0, 0)) * au::SqrtSq(MV(m, 0, 0), MV(m, 1, 0));
		m_scale.y = au::Sign(MV(m, 1, 1)) * au::SqrtSq(MV(m, 0, 1), MV(m, 1, 1));

		m_update_scale = false;
	}

	return m_scale;
}
const sf::Angle& Transform::GetRotation() const 
{
	if (m_update_rotation)
	{
		const float* m = GetTransform().getMatrix();

		m_rotation = sf::radians(std::atan2f(MV(m, 1, 0), MV(m, 1, 1)));

		m_update_rotation = false;
	}

	return m_rotation;
}