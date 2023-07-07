#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/System.hpp>

#include <Velox/System/Rectangle.hpp>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransformTranslation.h>

#include <Velox/UI/Components/Button.h>

#include <Velox/Input.hpp>
#include <Velox/Window.hpp>
#include <Velox/Config.hpp>

#include <Velox/ECS/SystemAction.h>

#include <Velox/World/EngineBinds.h>

namespace vlx::ui
{
	class VELOX_API ButtonSystem final : public SystemAction
	{
	public:
		ButtonSystem(
			EntityAdmin& entity_admin, LayerType id, 
			const Camera& camera, const EngineMouse& mouse, const MouseCursor& cursor);

	public:
		void Update() override;

	private:
		System<Renderable, GlobalTransformTranslation, Button>	m_buttons;

		System<Button, ButtonClick>		m_buttons_click;
		System<Button, ButtonPress>		m_buttons_press;
		System<Button, ButtonRelease>	m_buttons_release;
		System<Button, ButtonEnter>		m_buttons_enter;
		System<Button, ButtonExit>		m_buttons_exit;
	};
}