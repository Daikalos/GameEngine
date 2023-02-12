#include <Velox/Components/Transform.h>

using namespace vlx;

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
const sf::Vector2f& Transform::GetPosition() const 
{
	if (m_update_position)
	{
		const float* matrix = GetTransform().getMatrix();
		const auto mv = [&matrix](const int x, const int y)
		{
			return matrix[x + y * MATRIX_WIDTH];
		};

		m_position.x = mv(0, 3);
		m_position.y = mv(1, 3);

		m_update_position = false;
	}

	return m_position;
}
const sf::Vector2f& Transform::GetScale() const 
{
	if (m_update_scale)
	{
		const float* matrix = GetTransform().getMatrix();
		const auto mv = [&matrix](const int x, const int y)
		{
			return matrix[x + y * MATRIX_WIDTH];
		};

		m_scale.x = au::Sign(mv(0, 0)) * au::SP2(mv(0, 0), mv(1, 0));
		m_scale.y = au::Sign(mv(1, 1)) * au::SP2(mv(0, 1), mv(1, 1));

		m_update_scale = false;
	}

	return m_scale;
}
const sf::Angle& Transform::GetRotation() const 
{
	if (m_update_rotation)
	{
		const float* matrix = GetTransform().getMatrix();
		const auto mv = [&matrix](const int x, const int y)
		{ 
			return matrix[x + y * MATRIX_WIDTH];
		};

		m_rotation = sf::radians(std::atan2f(mv(1, 0), mv(1, 1)));

		m_update_rotation = false;
	}

	return m_rotation;
}