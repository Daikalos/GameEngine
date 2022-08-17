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

		void pre_update(float dt);
		void update(float dt);
		void fixed_update(float dt);
		void post_update(float dt, float interp);

		void draw();
		
		void register_states();

	private:
		Camera _camera;
		Window _window;
		InputHandler _input_handler;
		TextureHolder _texture_holder;
		StateStack _state_stack;
	};
}

