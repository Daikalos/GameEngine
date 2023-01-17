#include <Velox/World/World.h>

using namespace vlx;

World::World(const Window& window, const Time& time)
{
	AddSystem<ObjectSystem>(m_entity_admin);
	AddSystem<RelationSystem>(m_entity_admin);
	AddSystem<TransformSystem>(m_entity_admin);
	AddSystem<AnchorSystem>(m_entity_admin, window);
	AddSystem<RenderSystem>(m_entity_admin);
}

EntityAdmin& World::GetEntityAdmin() noexcept
{
	return m_entity_admin;
}
const EntityAdmin& World::GetEntityAdmin() const noexcept 
{
	return m_entity_admin;
}

void World::RemoveSystem(LayerType id)
{
	m_systems.erase(std::find_if(m_systems.begin(), m_systems.end(),
		[id](const auto& pair)
		{
			return pair.second->GetID() == id;
		}));

	m_sorted_systems.erase(id);
}

bool World::HasSystem(LayerType id) const
{
	return m_sorted_systems.contains(id);
}

void World::Update()
{
	for (const auto& system : m_sorted_systems)
		system.second.second->Update();
}

void World::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
	if (HasSystem<RenderSystem>())
		Get<RenderSystem>().draw(target, states);
}
