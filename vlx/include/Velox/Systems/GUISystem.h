#pragma once

#include <Velox/Components/Transform.h>
#include <Velox/Components/Sprite.h>

#include <Velox/Graphics.hpp>

namespace vlx::gui
{
	class GUISystem
	{
	public:



	private:
		System<Component, Transform, Sprite>	m_main_system;
		System<Component, Button>				m_button_system;
		System<Component, Text>					m_text_system;
	};
}