#include <Velox/World/SystemObject.h>

using namespace vlx;

SystemObject::SystemObject(EntityAdmin& entity_admin, const LayerType id)
	: m_entity_admin(&entity_admin), m_id(id) {}

constexpr LayerType SystemObject::GetID() const noexcept
{
	return m_id;
}
