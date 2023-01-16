#include <Velox/World/World.h>

using namespace vlx;

World::World(const Window& window, const Time& time)
{
	Add<ObjectSystem>(m_entity_admin);
	Add<RelationSystem>(m_entity_admin);
	Add<TransformSystem>(m_entity_admin);
	Add<AnchorSystem>(m_entity_admin, window);
	Add<RenderSystem>(m_entity_admin);
}

EntityAdmin& World::GetEntityAdmin() noexcept
{
	return m_entity_admin;
}
const EntityAdmin& World::GetEntityAdmin() const noexcept 
{
	return m_entity_admin;
}

void World::Update()
{
	for (const auto& system : m_sorted_systems)
		system.second.second->Update();
}

void World::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
	if (Has<RenderSystem>())
		Get<RenderSystem>().draw(target, states);
}
