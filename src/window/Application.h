#pragma once

#include <SFML/Graphics.hpp>

#include "../ecs/ECS.hpp"
#include "../scene/StateStack.h"
#include "../graphics/ResourceHolder.hpp"
#include "../utilities/Time.hpp"
#include "input/InputHandler.h"
#include "Window.h"
#include "Camera.h"

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
		Window			m_window;
		Camera			m_camera;
		StateStack		m_state_stack;
		InputHandler	m_input_handler;
		TextureHolder	m_texture_holder;
		FontHolder		m_font_holder;
		Time			m_time;
	};
}

