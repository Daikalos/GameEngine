#pragma once

#include <memory>

#include <Velox/Input/ControlMap.h>
#include <Velox/Window/Window.h>
#include <Velox/Window/Camera.h>
#include <Velox/Graphics/ResourceHolder.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	template<IntEnum ID>
	class StateStack;

	template<IntEnum ID = std::uint16_t>
	class State
	{
	public:
		using StateStack = typename StateStack<ID>;

		using Ptr = typename std::unique_ptr<State>;
		using Func = typename std::function<Ptr()>;

		struct Context // holds vital objects
		{
			Context(Window& window, Camera& camera, ControlMap& controls, TextureHolder& texture_holder, FontHolder& font_holder);

			Window* const				window;
			Camera*	const				camera;
			ControlMap* const			controls;
			TextureHolder* const		texture_holder;
			FontHolder*	const			font_holder;
		};

	public:
		explicit State(const ID id, StateStack& state_stack, Context context);
		virtual ~State();

	public:
		[[nodiscard]] const ID& GetID() const noexcept;

	protected:
		[[nodiscard]] const Context& GetContext() const;
		[[nodiscard]] StateStack& GetStack() const;

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
		virtual bool PostUpdate(Time& time);

		virtual void draw() = 0;

	private:
		ID			m_id;
		StateStack* m_state_stack;
		Context		m_context;
	};

	template<IntEnum ID>
	inline State<ID>::Context::Context(Window& window, Camera& camera, ControlMap& controls, TextureHolder& texture_holder, FontHolder& font_holder)
		: window(&window), camera(&camera), controls(&controls), texture_holder(&texture_holder), font_holder(&font_holder)
	{

	}

	template<IntEnum ID>
	inline State<ID>::State(const ID id, StateStack& state_stack, Context context)
		: m_id(id), m_state_stack(&state_stack), m_context(context)
	{

	}

	template<IntEnum ID>
	inline State<ID>::~State() = default;

	template<IntEnum ID>
	inline const ID& State<ID>::GetID() const noexcept
	{
		return m_id;
	}

	template<IntEnum ID>
	inline const State<ID>::Context& State<ID>::GetContext() const
	{
		return m_context;
	}
	template<IntEnum ID>
	inline StateStack<ID>& State<ID>::GetStack() const
	{
		return *m_state_stack;
	}

	template<IntEnum ID>
	inline void State<ID>::OnActivate()
	{

	}
	template<IntEnum ID>
	inline void State<ID>::OnDestroy()
	{

	}

	template<IntEnum ID>
	inline bool State<ID>::PreUpdate(Time& time)
	{
		return true;
	}
	template<IntEnum ID>
	inline bool State<ID>::FixedUpdate(Time& time)
	{
		return true;
	}
	template<IntEnum ID>
	inline bool State<ID>::PostUpdate(Time& time)
	{
		return true;
	}
}