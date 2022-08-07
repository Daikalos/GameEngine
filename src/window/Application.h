#pragma once

#include <SFML/Graphics.hpp>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceManager.h"
#include "StateStack.hpp"

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
		ResourceManager _resource_manager;
		StateStack _state_stack;
	};
}

