#pragma once

#include <SFML/Graphics.hpp>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceHolder.hpp"

#include "../base/StateStack.hpp"

namespace fge
{
	class Application
	{
	public:
		Application();
		~Application();

		void run();

	private:
		void process_input();
		void update(const float& dt);
		void draw();
		
		void register_states();

	private:
		Window _window;
		Camera _camera;
		InputHandler _input_handler;
		TextureHolder _texture_holder;
		StateStack _state_stack;
	};
}

