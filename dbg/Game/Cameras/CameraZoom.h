#pragma once

#include <Velox/Window/CameraBehavior.h>
#include <Velox/Window/Camera.h>

namespace vlx
{
	class CameraZoom final : public CameraBehavior
	{
	public:
		using CameraBehavior::CameraBehavior;

	private:
		bool HandleEvent(const sf::Event& event) override
		{ 
			return true;
		}

		bool Update(const Time& time) override
		{
			const Window* window = GetContext().window;
			const auto& mouse_input = GetContext().controls->Get<MouseInput>();

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