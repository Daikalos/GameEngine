#include <Velox/Graphics/Systems/CullingSystem.h>

using namespace vlx;

CullingSystem::CullingSystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera)
	: SystemAction(entity_admin, id), m_system(entity_admin, id), m_camera(&camera)
{
	m_system.All([this, &camera](std::span<const EntityID> entities, Object* objects, Transform* transforms, Sprite* sprites)
		{
			const sf::Vector2f camera_size = camera.GetSize() / camera.GetScale();
			const sf::Vector2f camera_pos = camera.GetPosition() - camera_size / 2.0f;

			const RectFloat camera_rect = 
			{ 
				camera_pos.x - LENIENCY,
				camera_pos.y - LENIENCY,
				camera_pos.x + camera_size.x + LENIENCY,
				camera_pos.y + camera_size.y + LENIENCY
			};

			const RectFloat gui_camera_rect =
			{
				-LENIENCY,
				-LENIENCY,
				camera.GetSize().x + LENIENCY,
				camera.GetSize().y + LENIENCY
			};

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				const sf::Vector2f pos = transforms[i].GetPosition();
				const sf::Vector2f size = sprites[i].GetSize();

				const RectFloat rect =
				{
					pos.x,
					pos.y,
					pos.x + size.x,
					pos.y + size.y
				};
				
				objects[i].IsVisible = objects[i].IsGUI ? gui_camera_rect.Overlaps(rect) : camera_rect.Overlaps(rect);
			}
		});
}

void CullingSystem::Update()
{
	m_entity_admin->RunSystems(GetID());
}

constexpr bool CullingSystem::IsRequired() const noexcept
{
	return false;
}

void CullingSystem::PreUpdate()
{
}

void CullingSystem::FixedUpdate()
{
}

void CullingSystem::PostUpdate()
{
}
