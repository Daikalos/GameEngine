#include <Velox/Components/Transform.h>

using namespace vlx;

Transform::Transform() 
	: m_origin(0, 0), m_position(0,0), m_rotation(), m_scale(1, 1)
{

}

Transform::~Transform()
{
	if (HasParent())
		DetachParent(*m_parent);

	for (Transform* child : m_children)
		child->m_parent = nullptr;
}

const sf::Transform& Transform::GetTransform() const
{
	if (m_update_model_transform)
		UpdateTransforms();

	return m_model_transform;
}
const sf::Transform& Transform::GetInverseTransform() const
{
	if (m_update_inverse_model_transform)
	{
		m_inverse_model_transform = GetTransform().getInverse();
		m_update_inverse_model_transform = false;
	}

	return m_inverse_model_transform;
}
const sf::Vector2f& Transform::GetOrigin() const
{
	return m_origin;
}
const sf::Vector2f& Transform::GetPosition() const 
{
	return GetTransform() * GetLocalPosition();
}
const sf::Vector2f& Transform::GetScale() const 
{
	return sf::Vector2f();
}
const sf::Angle& Transform::GetRotation() const 
{
	//m_global_transform.
	return sf::Angle();
}

const sf::Transform& Transform::GetLocalTransform() const
{
	if (m_update_local_transform)
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

		m_update_local_transform = false;
	}

	return m_local_transform;
}
const sf::Transform& Transform::GetInverseLocalTransform() const
{
	if (m_update_inverse_local_transform)
	{
		m_inverse_local_transform = GetLocalTransform().getInverse();
		m_update_inverse_local_transform = false;
	}

	return m_inverse_local_transform;
}
const sf::Vector2f& Transform::GetLocalOrigin() const
{
	return m_origin;
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

	m_update_local_transform = true;
	m_update_inverse_local_transform = true;

	UpdateRequired();
}
void Transform::SetPosition(const sf::Vector2f& position)
{
	m_position = position;

	m_update_local_transform = true;
	m_update_inverse_local_transform = true;

	UpdateRequired();
}
void Transform::SetScale(const sf::Vector2f& scale) 
{
	m_scale = scale;

	m_update_local_transform = true;
	m_update_inverse_local_transform = true;

	UpdateRequired();
}
void Transform::SetRotation(const sf::Angle angle)
{
	m_rotation = angle.wrapUnsigned();

	m_update_local_transform = true;
	m_update_inverse_local_transform = true;

	UpdateRequired();
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

constexpr bool Transform::HasParent() const noexcept
{
	return m_parent != nullptr;
}

const Transform& Transform::GetParent() const
{
	assert(HasParent());
	return *m_parent;
}
Transform& Transform::GetParent()
{
	return const_cast<Transform&>(static_cast<const Transform&>(*this).GetParent());
}

void Transform::AttachParent(Transform& parent)
{
	parent.AttachChild(*this);
}
Transform* Transform::DetachParent(Transform& parent)
{
	return parent.DetachChild(*this);
}

void Transform::AttachChild(Transform& child)
{
	if (this == &child || m_parent == &child) // sanity check
		return;

	if (child.HasParent()) // if child already has an attached parent
		child.DetachParent(*child.m_parent);

	child.m_parent = this;
	m_children.push_back(&child);

	child.UpdateRequired(); // now that the child has been attached, itself and its children needs their global matrix to be updated
}
Transform* Transform::DetachChild(Transform& child)
{
	auto found = std::find(m_children.begin(), m_children.end(), &child);

	if (found == m_children.end())
		return nullptr;

	child.m_parent = nullptr;
	m_children.erase(found);

	child.UpdateRequired();

	return &child;
}

void Transform::UpdateTransforms() const
{
	assert(m_update_model_transform);

	if (HasParent())
		ComputeTransform(m_parent->GetTransform());
	else
		ComputeTransform();

	m_update_model_transform = false;

	for (const Transform* child : m_children)
		child->UpdateTransforms();
}

void Transform::UpdateRequired() const
{
	m_update_model_transform = true;
	m_update_inverse_model_transform = true;

	for (const Transform* transform : m_children) // all of the children needs their global transform to be updated
		transform->UpdateRequired();
}

void Transform::ComputeTransform() const
{
	m_model_transform = GetLocalTransform();
}
void Transform::ComputeTransform(const sf::Transform& transform) const
{
	m_model_transform = transform * GetLocalTransform();
}