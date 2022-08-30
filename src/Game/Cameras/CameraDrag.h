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
			if (!GetContext().controls->Exists<MouseCursor>()) 
				return true;

			const Window* window = GetContext().window;

			const auto& mouse_input = GetContext().controls->Get<MouseInputBindable>();
			const auto& mouse_cursor = GetContext().controls->Get<MouseCursor>();

			sf::Vector2f position = GetCamera().GetPosition();

			if (mouse_input.Pressed(bn::Button::Drag))
				m_drag_pos = position + (mouse_cursor.GetPosition() / GetCamera().GetScale() / window->GetRatioCmp());
			if (mouse_input.Held(bn::Button::Drag))
				position = (m_drag_pos - (mouse_cursor.GetPosition() / GetCamera().GetScale() / window->GetRatioCmp()));

			GetCamera().SetPosition(position);

			return true;
		}

	private:
		sf::Vector2f m_drag_pos;
	};
}