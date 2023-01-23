#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/Identifiers.hpp>

#include <Velox/Graphics.hpp>
#include <Velox/Input.hpp>
#include <Velox/Config.hpp>

#include <Velox/World/SystemObject.h>

#include "../EngineBinds.h"

namespace vlx::gui
{

	class VELOX_API GUISystem : public SystemObject
	{
	public:
		GUISystem(EntityAdmin& entity_admin, const LayerType id, const ControlMap& controls);

	public:
		void Update() override;

	private:
		System<Transform, Button>	m_button_system;
		System<Transform, Label>	m_label_system;
	};
}