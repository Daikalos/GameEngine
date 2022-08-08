#pragma once

#include <memory>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceHolder.hpp"
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
			Context(Window& window, Camera& camera, InputHandler& input_handler, TextureHolder& texture_holder)
				: _window(&window), _camera(&camera), _input_handler(&input_handler), _texture_holder(&texture_holder) { }

			Window* _window;
			Camera* _camera;

			const InputHandler* _input_handler;
			const TextureHolder* _texture_holder;
		};

	public:
		explicit State(StateStack& state_stack, Context context)
			: _state_stack(&state_stack), _context(context) { }

		virtual ~State() {}

		virtual bool update(const float& dt) = 0;
		virtual bool handle_event(const sf::Event& event) = 0;
		virtual void draw() = 0;

		virtual void on_activate();
		virtual void on_destroy();
		virtual void is_paused();
		virtual void set_paused(bool flag);

	protected:
		void request_stack_push(States::ID state_id);
		void request_stack_pop();
		void request_stack_clear();

		const Context& get_context()
		{
			return _context;
		}

	private:
		StateStack* _state_stack;
		Context _context;
	};
}