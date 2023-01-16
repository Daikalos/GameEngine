#pragma once

#include <Velox/Graphics.hpp>
#include <Velox/Config.hpp>

#include "ISystemObject.h"

namespace vlx::gui
{

	class VELOX_API GUISystem : public ISystemObject
	{
	public:

		void Update() override;

	private:
		System<Button>	m_button_system;
		System<Label>	m_label_system;
	};
}