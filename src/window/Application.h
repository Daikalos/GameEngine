#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

//#include "../ecs/ECS.hpp"
#include "../scene/StateStack.h"
#include "../graphics/ResourceHolder.hpp"
#include "../utilities/Time.hpp"
#include "input/Binds.h"
#include "Window.h"
#include "Camera.h"

#include "input/Controls.hpp"
#include "input/KeyboardHandler.hpp"
#include "input/JoystickHandler.hpp"
#include "input/MouseHandler.hpp"

#include "../test/StateTest.h"

namespace fge
{
	class Application final
	{
	public:
		Application(std::string& name);
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
		void RegisterControls();

	private:
		Window			m_window;
		Camera			m_camera;
		StateStack		m_state_stack;
		TextureHolder	m_texture_holder;
		FontHolder		m_font_holder;
		Time			m_time;
		Controls		m_controls;
	};
}

