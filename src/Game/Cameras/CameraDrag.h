#pragma once

#include <Velox/Window/CameraBehaviour.hpp>
#include <Velox/Window/Camera.h>
#include <Velox/Window/Cameras.h>

namespace vlx
{
	class CameraDrag final : public CameraBehaviour
	{
	public:
		CameraDrag(camera::ID id, Camera& camera, Context context) : 
			CameraBehaviour(id, camera, context) { }

	private:
		bool HandleEvent(const sf::Event& event) override 
		{ 
			return true; 
		}

		bool Update(const Time& time) override
		{
			const Window* window = GetContext().window;

			const auto& mouse_input = GetContext().controls->Get<MouseInputBindable>();
			const auto& mouse_cursor = GetContext().controls->Get<MouseCursor>();

			sf::Vector2f position = GetCamera().GetPosition();

			if (mouse_input.Held(bn::Button::Drag))
				position -= sf::Vector2f(mouse_cursor.GetDelta()) / GetCamera().GetScale();

			GetCamera().SetPosition(position);

			return true;
		}

	private:
		sf::Vector2i m_drag_pos;
	};
}