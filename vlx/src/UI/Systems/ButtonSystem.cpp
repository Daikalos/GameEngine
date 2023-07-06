#include <Velox/UI/Systems/ButtonSystem.h>

using namespace vlx::ui;

ButtonSystem::ButtonSystem(EntityAdmin& entity_admin, LayerType id, const Camera& camera, const ControlMap& controls)
	: SystemAction(entity_admin, id),

	m_buttons(entity_admin, id),

	m_buttons_click(entity_admin, id),
	m_buttons_press(entity_admin, id),
	m_buttons_release(entity_admin, id),
	m_buttons_enter(entity_admin, id),
	m_buttons_exit(entity_admin, id)

{
	m_buttons.All(
		[&camera, &controls](std::span<const EntityID> entities, Renderable* renderables, GlobalTransformTranslation* gtts, Button* buttons)
		{
			const MouseInput& mouse_input = controls.Get<MouseInput>();
			const MouseCursor& mouse_cursor = controls.Get<MouseCursor>();

			const bool pressed	= mouse_input.Pressed(ebn::Button::GUIButton);
			const bool released = mouse_input.Released(ebn::Button::GUIButton);

			const Vector2i mouse_pos = mouse_cursor.GetPosition();
			const Vector2i global_mouse_pos = camera.GetMouseWorldPosition(mouse_pos);

			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				Renderable& renderable			= renderables[i];
				GlobalTransformTranslation& gtt	= gtts[i];
				Button& button					= buttons[i];

				button.m_call_pressed	= false;
				button.m_call_clicked	= false;
				button.m_call_released	= false;
				button.m_call_entered	= false;
				button.m_call_exited	= false;

				if (button.IsActive())
				{
					const Vector2i position	= Vector2i(gtt.GetPosition());
					const Vector2i size		= Vector2i(button.GetSize());

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

	m_buttons_click.Each(
		[](EntityID, Button& btn, ButtonClick& btn_click)
		{
			if (btn.m_call_clicked)
				btn_click.OnClick();
		});

	m_buttons_press.Each(
		[](EntityID, Button& btn, ButtonPress& btn_press)
		{
			if (btn.m_call_pressed)
				btn_press.OnPress();
		});

	m_buttons_release.Each(
		[](EntityID, Button& btn, ButtonRelease& btn_release)
		{
			if (btn.m_call_released)
				btn_release.OnRelease();
		});

	m_buttons_enter.Each(
		[](EntityID, Button& btn, ButtonEnter& btn_enter)
		{
			if (btn.m_call_entered)
				btn_enter.OnEnter();
		});

	m_buttons_exit.Each(
		[](EntityID, Button& btn, ButtonExit& btn_exit)
		{
			if (btn.m_call_exited)
				btn_exit.OnExit();
		});
}

void ButtonSystem::Update()
{
	Execute();
}
