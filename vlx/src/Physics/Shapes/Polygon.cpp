#include <Velox/Physics/Shapes/Polygon.h>

using namespace vlx;

constexpr auto Polygon::GetType() const noexcept -> Type
{
	return Type::Convex;
}

void Polygon::InitializeImpl(PhysicsBody& body) const noexcept
{

}

void Polygon::UpdateAABBImpl(const Transform& transform)
{

}

void Polygon::UpdateTransformImpl(const Transform& transform)
{
	m_transform = transform.GetTransform();
	//m_transform.translate(m_aabb.Size() / 2.0f);

	m_update_inverse = true;
}
