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

void Transform::SetOrigin(const sf::Vector2f& origin)
{
	m_origin = origin;
	Dirtify();
}
void Transform::SetPosition(const sf::Vector2f& position)
{
	m_position = position;
	Dirtify();
}
void Transform::SetScale(const sf::Vector2f& scale)
{
	m_scale = scale;
	Dirtify();
}
void Transform::SetRotation(const sf::Angle angle)
{
	m_rotation = angle.wrapUnsigned();
	Dirtify();
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

void Transform::UpdateRequired(const EntityAdmin& entity_admin, const Relation* relation) const
{
	if (m_update_model) // no need to update if already set
		return;

	m_update_model = true;
	m_update_inverse_model = true;

	if (relation == nullptr)
		return;

	for (const EntityID child_id : relation->GetChildren()) // all of the children needs their global transform to be updated
	{
		entity_admin.GetComponent<Transform>(child_id)
			.UpdateRequired(entity_admin, &entity_admin.GetComponent<Relation>(child_id));
	}
}
void Transform::UpdateTransforms(const EntityAdmin& entity_admin, const Relation* relation) const
{
	if (!m_update_model) // already up-to-date
		return;

	if (relation != nullptr && relation->HasParent())
	{
		const Transform& parent_transform = entity_admin.GetComponent<Transform>(relation->GetParent());
		const Relation& parent_relation = entity_admin.GetComponent<Relation>(relation->GetParent());

		parent_transform.UpdateTransforms(entity_admin, &parent_relation);
		ComputeTransform(parent_transform.GetTransform());

		const float* matrix = m_model_transform.getMatrix();

		const auto mv = [&matrix](const int x, const int y) -> float
		{
			constexpr int WIDTH = 4;
			return matrix[x + y * WIDTH];
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

void Transform::ComputeTransform() const
{
	m_model_transform = GetLocalTransform();
}
void Transform::ComputeTransform(const sf::Transform& transform) const
{
	m_model_transform = transform * GetLocalTransform();
}

void Transform::Dirtify() const
{
	m_update_local = true;
	m_update_inverse_local = true;

	m_update_global = true;
}