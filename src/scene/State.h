#pragma once

#include <memory>

#include "../window/Window.h"
#include "../window/Camera.h"

#include "../graphics/ResourceHolder.hpp"

#include "States.h"

namespace fge
{
	class StateStack;

	class State
	{
	public:
		using ptr = typename std::unique_ptr<State>;

		struct Context // holds vital objects
		{
			Context(Window& window, Camera& camera, InputHandler& input_handler, TextureHolder& texture_holder, FontHolder& font_holder)
				: window(&window), camera(&camera), input_handler(&input_handler), texture_holder(&texture_holder), font_holder(&font_holder) { }

			Window* const			window;
			Camera*	const			camera;
			InputHandler* const		input_handler;
			TextureHolder* const	texture_holder;
			FontHolder*	const		font_holder;
		};

	public:
		explicit State(StateStack& state_stack, Context context)
			: m_state_stack(&state_stack), m_context(context) { }

		virtual ~State() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(Time& time)				{ return true; }
		virtual bool Update(Time& time) = 0;
		virtual bool FixedUpdate(Time& time)				{ return true; }
		virtual bool PostUpdate(Time& time, float interp)	{ return true; }

		virtual void draw() = 0;

		virtual void OnActivate()	{}
		virtual void OnDestroy()	{}

		virtual bool IsPaused() const		{ m_state_stack->IsPaused(); }
		virtual void SetPaused(bool flag)	{ m_state_stack->SetPaused(flag); }

	protected:
		void RequestStackPush(States::ID state_id);
		void RequestStackPop();
		void RequestStackClear();

		const Context& GetContext()
		{
			return m_context;
		}

	private:
		StateStack* m_state_stack;
		Context m_context;
	};
}