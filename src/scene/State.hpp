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
		using Ptr = typename std::unique_ptr<State>;
		using Func = typename std::function<Ptr()>;

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
		explicit State(States::ID id, StateStack& state_stack, Context context)
			: m_id(id), m_state_stack(&state_stack), m_context(context) { }
		virtual ~State() {}

		States::ID GetId() const noexcept { return m_id; }

		virtual void OnActivate() {}
		virtual void OnDestroy() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(Time& time)					{ return true; }
		virtual bool Update(Time& time) = 0;
		virtual bool FixedUpdate(Time& time)				{ return true; }
		virtual bool PostUpdate(Time& time, float interp)	{ return true; }

		virtual void draw() = 0;

	protected:
		const Context& GetContext() const	{ return m_context; }
		StateStack& GetStack() const		{ return *m_state_stack; }

	private:
		States::ID	m_id;
		StateStack* m_state_stack;
		Context		m_context;
	};
}