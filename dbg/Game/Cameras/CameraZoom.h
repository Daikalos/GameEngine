#pragma once

#include <Velox/Window/CameraBehaviour.h>
#include <Velox/Window/Camera.h>
#include <Velox/Window/Cameras.h>

namespace vlx
{
	class CameraZoom final : public CameraBehaviour
	{
	public:
		CameraZoom(camera::ID id, Camera& camera, Context context) :
			CameraBehaviour(id, camera, context) { }

	private:
		bool HandleEvent(const sf::Event& event) override
		{ 
			return true;
		}

		bool Update(const Time& time) override
		{
			const Window* window = GetContext().window;
			const auto& mouse_input = GetContext().controls->Get<MouseBindable>();

			sf::Vector2f scale = GetCamera().GetScale();

			if (mouse_input.ScrollUp())
				scale += sf::Vector2f(0.1f, 0.1f);
			if (mouse_input.ScrollDown())
				scale -= sf::Vector2f(0.1f, 0.1f);

			GetCamera().SetScale(scale);

			return true;
		}
	};
}