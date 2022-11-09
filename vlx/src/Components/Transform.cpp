#include <Velox/Components/Transform.h>

using namespace vlx;

Transform::Transform() 
	: m_origin(0, 0), m_position(0,0), m_scale(1, 1), m_rotation()
{

}

Transform::Transform(const sf::Vector2f& position, const sf::Vector2f& scale, const sf::Angle& rotation)
	: m_origin(0, 0), m_position(position), m_scale(scale), m_rotation(rotation), m_global_position(position), m_global_scale(scale), m_global_rotation(rotation)
{

}
Transform::Transform(const sf::Vector2f& position, const sf::Vector2f& scale)
	: Transform(position, scale, sf::radians(0.0f))
{

}
Transform::Transform(const sf::Vector2f& position, const sf::Angle& rotation)
	: Transform(position, { 1.0f, 1.0f }, rotation)
{

}
Transform::Transform(const sf::Vector2f& position)
	: Transform(position, { 1.0f, 1.0f }, sf::radians(0.0f))
{

}

const sf::Transform& Transform::GetTransform() const
{
	return m_model_transform;
}
const sf::Transform& Transform::GetInverseTransform() const
{
	if (m_update_inverse_model)
	{
		m_inverse_model_transform = GetTransform().getInverse();
		m_update_inverse_model = false;
	}

	return m_inverse_model_transform;
}
const sf::Vector2f& Transform::GetOrigin() const
{
	return m_origin;
}
const sf::Vector2f& Transform::GetPosition() const 
{
	return m_global_position;
}
const sf::Vector2f& Transform::GetScale() const 
{
	return m_global_scale;
}
const sf::Angle& Transform::GetRotation() const 
{
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
	if (m_update_inverse_local)
	{
		m_inverse_local_transform = GetLocalTransform().getInverse();
		m_update_inverse_local = false;
	}

	return m_inverse_local_transform;
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

void Transform::SetOrigin(const EntityAdmin& entity_admin, const sf::Vector2f& origin)
{
	m_origin = origin;

	m_update_local = true;
	m_update_inverse_local = true;

	UpdateRequired(entity_admin);
}
void Transform::SetPosition(const EntityAdmin& entity_admin, const sf::Vector2f& position, bool global)
{
	if (global && HasParent())
	{
		Transform& parent = entity_admin.GetComponent<Transform>(m_parent);

		parent.UpdateTransforms(entity_admin); // update transform
		m_position = parent.GetInverseTransform() * position;
	}
	else
	{
		m_position = position;
	}

	m_update_local = true;
	m_update_inverse_local = true;

	UpdateRequired(entity_admin);
}
void Transform::SetScale(const EntityAdmin& entity_admin, const sf::Vector2f& scale)
{
	m_scale = scale;

	m_update_local = true;
	m_update_inverse_local = true;

	UpdateRequired(entity_admin);
}
void Transform::SetRotation(const EntityAdmin& entity_admin, const sf::Angle angle)
{
	m_rotation = angle.wrapUnsigned();

	m_update_local = true;
	m_update_inverse_local = true;

	UpdateRequired(entity_admin);
}

void Transform::Move(const EntityAdmin& entity_admin, const sf::Vector2f& move)
{
	SetPosition(entity_admin, GetLocalPosition() + move);
}
void Transform::Scale(const EntityAdmin& entity_admin, const sf::Vector2f& factor)
{
	const sf::Vector2f scale = GetLocalScale();
	SetScale(entity_admin, { scale.x * factor.x, scale.y * factor.y });
}
void Transform::Rotate(const EntityAdmin& entity_admin, const sf::Angle angle)
{
	SetRotation(entity_admin, GetLocalRotation() + angle);
}

void Transform::OnAttach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Transform>& child)
{
	static_cast<Transform&>(child).UpdateRequired(entity_admin); // crtp
}
void Transform::OnDetach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Transform>& child)
{
	static_cast<Transform&>(child).UpdateRequired(entity_admin);
}

void Transform::UpdateTransforms(const EntityAdmin& entity_admin) const
{
	if (!m_update_model) // already up-to-date
		return;

	if (HasParent())
	{
		const Transform& parent_transform = entity_admin.GetComponent<Transform>(m_parent);

		parent_transform.UpdateTransforms(entity_admin);
		ComputeTransform(parent_transform.GetTransform());

		const float* matrix = m_model_transform.getMatrix();

		const auto mv = [&matrix](const int x, const int y) -> float
		{
			constexpr int width = 4;
			return matrix[x + y * width];
		};

		m_global_position.x = mv(0, 3);
		m_global_position.y = mv(1, 3);

		m_global_scale.x = au::Sign(mv(0, 0)) * au::SP2(mv(0, 0), mv(1, 0));
		m_global_scale.y = au::Sign(mv(1, 1)) * au::SP2(mv(0, 1), mv(1, 1));

		m_global_rotation = sf::radians(std::atan2f(mv(1, 0), mv(1, 1)));
	}
	else
	{
		ComputeTransform();

		m_global_position = m_position;
		m_global_scale = m_scale;
		m_global_rotation = m_rotation;
	}

	m_update_model = false;
}

void Transform::UpdateRequired(const EntityAdmin& entity_admin) const
{
	if (m_update_model) // no need to update if already set
		return;

	m_update_model = true;
	m_update_inverse_model = true;

	for (const EntityID entity : m_children) // all of the children needs their global transform to be updated
	{
		entity_admin.GetComponent<Transform>(entity)
			.UpdateRequired(entity_admin);
	}
}

void Transform::ComputeTransform() const
{
	m_model_transform = GetLocalTransform();
}
void Transform::ComputeTransform(const sf::Transform& transform) const
{
	m_model_transform = transform * GetLocalTransform();
}