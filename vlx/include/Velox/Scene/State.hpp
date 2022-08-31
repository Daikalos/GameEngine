#pragma once

#include <memory>

#include <Velox/Input/ControlMap.h>
#include <Velox/Window/Window.h>
#include <Velox/Window/Camera.h>
#include <Velox/Graphics/ResourceHolder.hpp>

#include "States.h"

namespace vlx
{
	class StateStack;

	class State
	{
	public:
		using Ptr = typename std::unique_ptr<State>;
		using Func = typename std::function<Ptr()>;

		struct Context // holds vital objects
		{
			Context(Window& window, Camera& camera, ControlMap& controls, TextureHolder& texture_holder, FontHolder& font_holder)
				: window(&window), camera(&camera), controls(&controls), texture_holder(&texture_holder), font_holder(&font_holder) { }

			Window* const				window;
			Camera*	const				camera;
			ControlMap* const			controls;
			TextureHolder* const		texture_holder;
			FontHolder*	const			font_holder;
		};

	public:
		explicit State(state::ID id, StateStack& state_stack, Context context);
		virtual ~State() = default;

	public:
		state::ID GetId() const noexcept;

	protected:
		const Context& GetContext() const;
		StateStack& GetStack() const;

	public:
		////////////////////////////////////////////////////////////
		// OnActivate is called whenever the state is put as 
		// last in the stack
		////////////////////////////////////////////////////////////
		virtual void OnActivate();

		////////////////////////////////////////////////////////////
		// OnDestroy is called when the state is removed from
		// the stack
		////////////////////////////////////////////////////////////
		virtual void OnDestroy();

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(Time& time);
		virtual bool Update(Time& time) = 0;
		virtual bool FixedUpdate(Time& time);
		virtual bool PostUpdate(Time& time, float interp);

		virtual void draw() = 0;

	private:
		state::ID	m_id;
		StateStack* m_state_stack;
		Context		m_context;
	};

	inline State::State(state::ID id, StateStack& state_stack, Context context)
		: m_id(id), m_state_stack(&state_stack), m_context(context) { }

	inline state::ID State::GetId() const noexcept
	{ 
		return m_id; 
	}

	inline const State::Context& State::GetContext() const { return m_context; }
	inline StateStack& State::GetStack() const { return *m_state_stack; }

	inline void State::OnActivate() {}
	inline void State::OnDestroy() {}

	inline bool State::PreUpdate(Time& time)					{ return true; }
	inline bool State::FixedUpdate(Time& time)					{ return true; }
	inline bool State::PostUpdate(Time& time, float interp)	{ return true; }
}