#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include <Velox/System/Rectangle.hpp>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransformTranslation.h>

#include <Velox/Graphics/GUI/Button.h>
#include <Velox/Graphics/GUI/Label.h>

#include <Velox/Input.hpp>
#include <Velox/Window.hpp>
#include <Velox/Config.hpp>

#include <Velox/ECS/SystemAction.h>

#include <Velox/World/EngineBinds.h>

namespace vlx::gui
{
	class VELOX_API GUISystem final : public SystemAction
	{
	public:
		GUISystem(EntityAdmin& entity_admin, LayerType id, const Camera& camera, const ControlMap& controls);

	public:
		void Update() override;

	private:
		System<Renderable, GlobalTransformTranslation, Button>	m_button_system;
		System<Renderable, GlobalTransformTranslation, Label>	m_label_system;
	};
}