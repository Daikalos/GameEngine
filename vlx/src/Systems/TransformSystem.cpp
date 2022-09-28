#include <Velox/Systems/TransformSystem.h>

using namespace vlx;

TransformSystem::TransformSystem(EntityAdmin* entity_admin)
	: m_system(*entity_admin, LYR_Transform)
{
	//m_system.Action([])
}