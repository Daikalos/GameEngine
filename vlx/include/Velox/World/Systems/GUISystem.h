#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include <Velox/Graphics.hpp>
#include <Velox/Input.hpp>
#include <Velox/Config.hpp>

#include <Velox/World/SystemObject.h>

namespace vlx::gui
{

	class VELOX_API GUISystem : public SystemObject
	{
	public:
		GUISystem(EntityAdmin& entity_admin, const LayerType id);

	public:
		void Update() override;

	private:
		System<Container>	m_container_system;
		System<Button>		m_button_system;
		System<Label>		m_label_system;
	};
}