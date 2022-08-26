#pragma once

#include "../CameraBehaviour.hpp"
#include "../cm.h"
#include "../Camera.h"

namespace vlx
{
	class CameraDrag final : public CameraBehaviour
	{
	public:
		CameraDrag(cm::ID id, Camera& camera, Context context) : 
			CameraBehaviour(id, camera, context) { }

	private:
		bool HandleEvent(const sf::Event& event) override 
		{ 
			return true; 
		}

		bool Update(const Time& time) override
		{
			const Window* window = GetContext().window;
			const InputHandler* input_handler = GetContext().input_handler;

			sf::Vector2f position = GetCamera().GetPosition();

			if (input_handler->GetButtonPressed(Binds::Button::Drag))
				m_drag_pos = GetCamera().GetMouseWorldPosition(*window);
			if (input_handler->GetButtonHeld(Binds::Button::Drag))
				position += (m_drag_pos - GetCamera().GetMouseWorldPosition(*window));

			GetCamera().SetPosition(position);

			return true;
		}

	private:
		sf::Vector2f m_drag_pos;
	};
}