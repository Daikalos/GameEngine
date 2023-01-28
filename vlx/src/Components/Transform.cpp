#include <Velox/Components/Transform.h>

using namespace vlx;

Transform::Transform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation)
	: m_origin(0, 0), m_position(position), m_scale(scale), m_rotation(rotation), m_global_position(position), m_global_scale(scale), m_global_rotation(rotation)
{

}

Transform::Transform() 
	: Transform({ 0, 0 }, { 1.0f, 1.0f }, sf::radians(0.0f)) { }
Transform::Transform(const sf::Vector2f& position, const sf::Vector2f& scale)
	: Transform(position, scale, sf::radians(0.0f)) {}
Transform::Transform(const sf::Vector2f& position, const sf::Angle& rotation) 
	: Transform(position, { 1.0f, 1.0f }, rotation) {}
Transform::Transform(const sf::Vector2f& position) 
	: Transform(position, { 1.0f, 1.0f }, sf::radians(0.0f)) {}

const sf::Transform& Transform::GetTransform() const
{
	return m_global_transform;
}
const sf::Transform& Transform::GetInverseTransform() const
{
	if (m_update_global_inverse)
	{
		m_global_inverse_transform = GetTransform().getInverse();
		m_update_global_inverse = false;
	}

	return m_global_inverse_transform;
}
const sf::Vector2f& Transform::GetOrigin() const
{
	return m_origin;
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

		m_global_position.x = mv(0, 3);
		m_global_position.y = mv(1, 3);

		m_update_position = false;
	}

	return m_global_position;
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

		m_global_scale.x = au::Sign(mv(0, 0)) * au::SP2(mv(0, 0), mv(1, 0));
		m_global_scale.y = au::Sign(mv(1, 1)) * au::SP2(mv(0, 1), mv(1, 1));

		m_update_scale = false;
	}

	return m_global_scale;
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

		m_global_rotation = sf::radians(std::atan2f(mv(1, 0), mv(1, 1)));

		m_update_rotation = false;
	}

	return m_global_rotation;
}

const sf::Transform& Transform::GetLocalTransform() const
{
	if (m_update_local)
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

		m_local_transform = sf::Transform( sxc, sys, tx,
										  -sxs, syc, ty,
										   0.f, 0.f, 1.f);

		m_update_local = false;
	}

	return m_local_transform;
}
const sf::Transform& Transform::GetInverseLocalTransform() const
{
	if (m_update_local_inverse)
	{
		m_local_inverse_transform = GetLocalTransform().getInverse();
		m_update_local_inverse = false;
	}

	return m_local_inverse_transform;
}
const sf::Vector2f& Transform::GetLocalPosition() const
{
	return m_position;
}
const sf::Vector2f& Transform::GetLocalScale() const
{
	return m_scale;
}
const sf::Angle& Transform::GetLocalRotation() const
{
	return m_rotation;
}

void Transform::SetOrigin(const sf::Vector2f& origin)
{
	m_origin = origin;

	m_update_local = true;
	m_update_local_inverse = true;

	m_dirty = true;
}
void Transform::SetPosition(const sf::Vector2f& position)
{
	m_position = position;

	m_update_local = true;
	m_update_local_inverse = true;

	m_dirty = true;
}
void Transform::SetScale(const sf::Vector2f& scale)
{
	m_scale = scale;

	m_update_local = true;
	m_update_local_inverse = true;

	m_dirty = true;
}
void Transform::SetRotation(const sf::Angle angle)
{
	m_rotation = angle.wrapUnsigned();
	
	m_update_local = true;
	m_update_local_inverse = true;

	m_dirty = true;
}

void Transform::Move(const sf::Vector2f& move)
{
	SetPosition(GetLocalPosition() + move);
}
void Transform::Scale(const sf::Vector2f& factor)
{
	const sf::Vector2f scale = GetLocalScale();
	SetScale({ scale.x * factor.x, scale.y * factor.y });
}
void Transform::Rotate(const sf::Angle angle)
{
	SetRotation(GetLocalRotation() + angle);
}