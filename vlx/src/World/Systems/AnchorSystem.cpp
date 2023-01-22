#include <Velox/World/Systems/AnchorSystem.h>

using namespace vlx;

AnchorSystem::AnchorSystem(EntityAdmin& entity_admin, const LayerType id, const Window& window)
	: SystemObject(entity_admin, id), m_system(entity_admin, id)
{
	m_system.Action([this, &window](std::span<const EntityID> entities, Transform* transforms, gui::Anchor* anchors)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				switch (anchors[i].anchor)
				{
				case gui::AnchorPoint::TopLeft:

					break;
				case gui::AnchorPoint::TopRight:

					break;
				case gui::AnchorPoint::BotLeft:

					break;
				case gui::AnchorPoint::BotRight:

					break;
				case gui::AnchorPoint::Middle:

					break;
				}
			}
		});
}

void AnchorSystem::Update()
{
	m_entity_admin->RunSystems(GetID());
}
