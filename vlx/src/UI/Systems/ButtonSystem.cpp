#include <Velox/UI/Systems/ButtonSystem.h>

using namespace vlx::ui;

ButtonSystem::ButtonSystem(EntityAdmin& entity_admin, LayerType id, 
	const Camera& camera, const EngineMouse& mouse, const MouseCursor& cursor) : 
	
	SystemAction(entity_admin, id), m_buttons(entity_admin, id), m_register(entity_admin, id)

{
	m_buttons.All(
		[&camera, &mouse, &cursor](std::span<const EntityID> entities, Object* os, Renderable* rs, GlobalTransformTranslation* gs, UIBase* us, Button* bs)
		{
			const bool pressed	= mouse.Pressed(ebn::Button::GUIButton);
			const bool released = mouse.Released(ebn::Button::GUIButton);

			const Vector2i mouse_pos = cursor.GetPosition();
			const Vector2i global_mouse_pos = camera.GetMouseWorldPosition(mouse_pos);

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				auto& object		= os[i];
				auto& renderable	= rs[i];
				auto& translation	= gs[i];
				auto& base			= us[i];
				auto& button		= bs[i];

				button.m_flags = NULL;

				if (object.GetActive())
				{
					const Vector2i position	= Vector2i(translation.GetPosition());
					const Vector2i size		= Vector2i(base.GetSize());

					const RectInt rectangle(position, size);
					const bool within_bounds = rectangle.Contains(renderable.IsGUI ? mouse_pos : global_mouse_pos);

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

	m_register.Each([this](EntityID entity_id, Button& button)
		{
			if (button.m_flags)
				m_button_callbacks.emplace_back(entity_id, button.m_flags);
		});
}

void ButtonSystem::Update()
{
	Execute();
	ExecuteCallbacks();
}

void ButtonSystem::ExecuteCallbacks()
{
	for (ButtonEntityCallback& callback : m_button_callbacks)
	{
		if ((callback.flags & Button::E_Clicked) == Button::E_Clicked)
			CallClick(callback.entity_id);

		if ((callback.flags & Button::E_Pressed) == Button::E_Pressed)
			CallPress(callback.entity_id);

		if ((callback.flags & Button::E_Released) == Button::E_Released)
			CallRelease(callback.entity_id);

		if ((callback.flags & Button::E_Entered) == Button::E_Entered)
			CallEnter(callback.entity_id);

		if ((callback.flags & Button::E_Exited) == Button::E_Exited)
			CallExit(callback.entity_id);
	}

	m_button_callbacks.clear();
}

void ButtonSystem::CallClick(EntityID entity_id) const
{
	ButtonClick* event = m_entity_admin->TryGetComponent<ButtonClick>(entity_id);
	if (event != nullptr)
		event->OnClick();
}
void ButtonSystem::CallPress(EntityID entity_id) const
{
	ButtonPress* event = m_entity_admin->TryGetComponent<ButtonPress>(entity_id);
	if (event != nullptr)
		event->OnPress();
}
void ButtonSystem::CallRelease(EntityID entity_id) const
{
	ButtonRelease* event = m_entity_admin->TryGetComponent<ButtonRelease>(entity_id);
	if (event != nullptr)
		event->OnRelease();
}
void ButtonSystem::CallEnter(EntityID entity_id) const
{
	ButtonEnter* event = m_entity_admin->TryGetComponent<ButtonEnter>(entity_id);
	if (event != nullptr)
		event->OnEnter();
}
void ButtonSystem::CallExit(EntityID entity_id) const
{
	ButtonExit* event = m_entity_admin->TryGetComponent<ButtonExit>(entity_id);
	if (event != nullptr)
		event->OnExit();
}
