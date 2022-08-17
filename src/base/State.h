#pragma once

#include <memory>

#include "../window/Window.h"
#include "../window/Camera.h"
#include "../window/InputHandler.h"
#include "../window/ResourceHolder.hpp"

#include "States.h"

namespace fge
{
	class StateStack;

	class State
	{
	public:
		using ptr = std::unique_ptr<State>;

		struct Context // holds vital objects
		{
			Context(Window& window, Camera& camera, InputHandler& input_handler, TextureHolder& texture_holder, FontHolder& font_holder)
				: _window(&window), _camera(&camera), _input_handler(&input_handler), _texture_holder(&texture_holder), _font_holder(&font_holder) { }

			Window* _window;
			Camera* _camera;
			InputHandler* _input_handler;
			TextureHolder* _texture_holder;
			FontHolder* _font_holder;
		};

	public:
		explicit State(StateStack& state_stack, Context context)
			: _state_stack(&state_stack), _context(context) { }

		virtual ~State() {}

		virtual bool handle_event(const sf::Event& event) = 0;

		virtual bool pre_update(float dt)					{ return true; }
		virtual bool update(float dt) = 0;
		virtual bool fixed_update(float dt)					{ return true; }
		virtual bool post_update(float dt, float interp)	{ return true; }

		virtual void draw() = 0;

		virtual void on_activate()	{}
		virtual void on_destroy()	{}

		virtual bool is_paused() const		{ _state_stack->is_paused(); }
		virtual void set_paused(bool flag)	{ _state_stack->set_paused(flag); }

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