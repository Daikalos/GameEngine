#pragma once

#include <SFML/Graphics.hpp>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceHolder.hpp"

#include "../ecs/ECS.hpp"
#include "../base/StateStack.h"

namespace fge
{
	class Application
	{
	public:
		Application(const std::string& name);
		~Application();

		void run();

	private:
		void process_input();
		void update(float dt);
		void draw(float interp);
		
		void register_states();

	private:
		Camera _camera;
		Window _window;
		InputHandler _input_handler;
		TextureHolder _texture_holder;
		StateStack _state_stack;
	};
}

