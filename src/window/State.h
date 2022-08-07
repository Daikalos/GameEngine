#pragma once

#include <memory>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceManager.h"
#include "States.h"

namespace fge
{
	class StateStack;

	class State
	{
	public:
		typedef std::unique_ptr<State> ptr;

		struct Context // holds vital objects
		{
			Context(Window& window, Camera& camera, InputHandler& input_handler, ResourceManager& resource_manager) 
				: _window(&window), _camera(&camera), _input_handler(&input_handler), _resource_manager(&resource_manager) { }

			const Window* _window;
			const Camera* _camera;
			const InputHandler* _input_handler;
			const ResourceManager* _resource_manager;
		};

	public:
		explicit State(StateStack& state_stack, Context context)
			: _state_stack(&state_stack), _context(context) {}

		virtual ~State() {}

		virtual void draw() = 0;
		virtual bool update(const float& dt) = 0;
		virtual bool handle_event(const sf::Event& event) = 0;

		virtual void on_activate();
		virtual void on_destroy();
		virtual void is_paused();
		virtual void set_paused(bool flag);

	protected:
		void request_stack_push(States::ID state_id);
		void request_stack_pop();
		void request_stack_clear();

		const Context& get_context() const
		{
			return _context;
		}

	private:
		StateStack* _state_stack;
		Context _context;
	};
}