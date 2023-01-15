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
		System<Transform, Sprite>	m_main_system;
		System<Button>				m_button_system;
		System<Label>				m_text_system;
	};
}