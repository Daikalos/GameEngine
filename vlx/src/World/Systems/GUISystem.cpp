#include <Velox/World/Systems/GUISystem.h>

using namespace vlx::gui;

GUISystem::GUISystem(EntityAdmin& entity_admin, const LayerType id, const ControlMap& controls)
	: SystemObject(entity_admin, id),
	m_button_system(entity_admin, id),
	m_label_system(entity_admin, id)
{
	m_button_system.Action([&controls](std::span<const EntityID> entities, Transform* transforms, Button* buttons)
		{
			const MouseInput& mouse_input = controls.Get<MouseInput>();
			const MouseCursor& mouse_cursor = controls.Get<MouseCursor>();

			const bool pressed	= mouse_input.Pressed(ebn::Button::GUIButton);
			const bool released = mouse_input.Released(ebn::Button::GUIButton);

			const sf::Vector2i& mouse_pos = mouse_cursor.GetPosition();

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Transform& transform = transforms[i];
				Button& button = buttons[i];

				const auto& position	= sf::Vector2i(transform.GetPosition());
				const auto& size		= sf::Vector2i(button.GetSize());

				if (pressed || released)
				{
					sf::Rect rectangle(position, size);
					if (rectangle.contains(mouse_pos))
					{
						if (pressed)
							button.Pressed();
						if (released)
							button.Released();
					}
				}
			}
		});
}

void GUISystem::Update()
{
	m_entity_admin->RunSystems(GetID());
}
