#pragma once

#include "../CameraBehaviour.hpp"
#include "../cm.h"
#include "../Camera.h"

namespace fge
{
	class CameraZoom final : public CameraBehaviour
	{
	public:
		CameraZoom(cm::ID id, Camera& camera, Context context) :
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

			sf::Vector2f scale = GetCamera().GetScale();

			if (input_handler->GetScrollUp())
				scale += sf::Vector2f(0.1f, 0.1f);
			if (input_handler->GetScrollDown())
				scale -= sf::Vector2f(0.1f, 0.1f);

			GetCamera().SetScale(scale);

			return true;
		}
	};
}