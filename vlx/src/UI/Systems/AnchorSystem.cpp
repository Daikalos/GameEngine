#include <Velox/UI/Systems/AnchorSystem.h>

using namespace vlx;

AnchorSystem::AnchorSystem(EntityAdmin& entity_admin, LayerType id, const Window& window)
	: SystemAction(entity_admin, id), m_system(entity_admin, id)
{
	m_system.All([this, &window](std::span<const EntityID> entities, Transform* transforms, ui::Anchor* anchors)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				switch (anchors[i].anchor)
				{
				case ui::AnchorPoint::TopLeft:

					break;
				case ui::AnchorPoint::TopRight:

					break;
				case ui::AnchorPoint::BotLeft:

					break;
				case ui::AnchorPoint::BotRight:

					break;
				case ui::AnchorPoint::Middle:

					break;
				}
			}
		});
}

void AnchorSystem::Update()
{
	Execute();
}
