#pragma once

#include <memory>

#include "Window.h"
#include "InputHandler.h"
#include "ResourceManager.h"

namespace fge
{
	class StateStack; // forward declare

	class State
	{
	public:
		typedef std::unique_ptr<State> ptr;

		struct Context // holds vital objects
		{
			Context(Window& window, InputHandler& input_handler, ResourceManager& resource_manager) 
				: window(&window), input_handler(&input_handler), resource_manager(&resource_manager) { }

			Window* window;
			InputHandler* input_handler;
			ResourceManager* resource_manager;
		};

	public:
		explicit State(StateStack& stateStack, Context context)
			: _stateStack(&stateStack), _context(context) {}

		virtual ~State() {}

		virtual void draw() = 0;
		virtual bool update(const float& dt) = 0;
		virtual bool handle_event(const sf::Event& event) = 0;

		virtual void on_activate();
		virtual void on_destroy();
		virtual void is_paused();
		virtual void set_paused(bool flag);

	private:
		StateStack* _stateStack;
		Context _context;
	};
}