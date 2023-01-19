#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include <Velox/Graphics.hpp>
#include <Velox/Input.hpp>
#include <Velox/Config.hpp>

#include "ISystemObject.h"

namespace vlx::gui
{

	class VELOX_API GUISystem : public ISystemObject
	{
	public:
		GUISystem(EntityAdmin& entity_admin, const ControlMap& controls);

	public:
		constexpr LayerType GetID() const noexcept override;

	public:
		void Update() override;

	private:
		System<Button>	m_button_system;
		System<Label>	m_label_system;
	};
}