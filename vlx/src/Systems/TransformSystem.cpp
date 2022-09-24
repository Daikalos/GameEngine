#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin* entity_admin)
	: m_system(*entity_admin, TRANSFORM_LAYER)
{
	//m_system.Action([])
}