#include <Velox/Graphics/Systems/GUISystem.h>

using namespace vlx::gui;

GUISystem::GUISystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera, const ControlMap& controls)
	: SystemAction(entity_admin, id),
	m_button_system(entity_admin, id),
	m_label_system(entity_admin, id)
{
	m_button_system.All([&camera, &controls](std::span<const EntityID> entities, Renderable* renderables, Transform* transforms, Button* buttons)
		{
			const MouseInput& mouse_input = controls.Get<MouseInput>();
			const MouseCursor& mouse_cursor = controls.Get<MouseCursor>();

			const bool pressed	= mouse_input.Pressed(ebn::Button::GUIButton);
			const bool released = mouse_input.Released(ebn::Button::GUIButton);

			const Vector2i mouse_pos = mouse_cursor.GetPosition();
			const Vector2i global_mouse_pos = camera.GetMouseWorldPosition(mouse_pos);

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Renderable& renderable	= renderables[i];
				Transform& transform	= transforms[i];
				Button& button			= buttons[i];

				const auto& position	= Vector2i(transform.GetPosition());
				const auto& size		= Vector2i(button.GetSize());

				if (button.IsActive())
				{
					RectInt rectangle(position, size);
					bool within_bounds = rectangle.Contains(renderable.IsGUI ? mouse_pos : global_mouse_pos);

					if (within_bounds)
					{
						button.Enter();

						if (pressed)
							button.Press();

						if (released) // if released within bounds, click will occur
							button.Click();
					}
					else
					{
						button.Exit();
					}
				}

				if (released)
					button.Release();
			}
		});

	m_label_system.All([](std::span<const EntityID> entities, Renderable* renderables, Transform* transforms, Label* labels)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{

			}
		});
}

constexpr bool GUISystem::IsRequired() const noexcept
{
	return false;
}

void GUISystem::PreUpdate()
{

}

void GUISystem::Update()
{
	Execute();
}

void GUISystem::FixedUpdate()
{

}

void GUISystem::PostUpdate()
{

}
