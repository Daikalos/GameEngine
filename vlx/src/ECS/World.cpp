#include <Velox/ECS/World.h>

using namespace vlx;

World::World() : m_entity_admin(), m_object_system(m_entity_admin), m_transform_system(m_entity_admin), m_render_system(m_entity_admin)
{
	
}

EntityAdmin& World::GetEntityAdmin() noexcept
{
	return m_entity_admin;
}
ObjectSystem& World::GetObjectSystem() noexcept 
{
	return m_object_system;
}
TransformSystem& World::GetTransformSystem() noexcept
{
	return m_transform_system;
}
RenderSystem& World::GetRenderSystem() noexcept
{
	return m_render_system;
}

const EntityAdmin& World::GetEntityAdmin() const noexcept 
{
	return m_entity_admin;
}
const ObjectSystem& World::GetObjectSystem() const noexcept 
{
	return m_object_system;
}
const TransformSystem& World::GetTransformSystem() const noexcept 
{
	return m_transform_system;
}
const RenderSystem& World::GetRenderSystem() const noexcept
{
	return m_render_system;
}