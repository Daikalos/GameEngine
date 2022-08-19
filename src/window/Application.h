#pragma once

#include <SFML/Graphics.hpp>

#include "Window.h"
#include "Camera.h"
#include "input/InputHandler.h"

#include "../ecs/ECS.hpp"
#include "../scene/StateStack.h"
#include "../graphics/ResourceHolder.hpp"
#include "../utilities/Time.hpp"

namespace fge
{
	class Application final
	{
	public:
		Application(const std::string& name);
		~Application();

		void Run();

	private:
		void ProcessEvents();

		void PreUpdate();
		void Update();
		void FixedUpdate();
		void PostUpdate(float interp);

		void Draw();
		
		void RegisterStates();

	private:
		Time			m_time;
		Camera			m_camera;
		Window			m_window;
		InputHandler	m_input_handler;
		TextureHolder	m_texture_holder;
		FontHolder		m_font_holder;
		StateStack		m_state_stack;
	};
}

