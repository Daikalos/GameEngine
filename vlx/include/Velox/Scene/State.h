#pragma once

#include <memory>

#include <Velox/Input/ControlMap.h>
#include <Velox/Window/Window.h>
#include <Velox/Window/Camera.h>
#include <Velox/Graphics/ResourceHolder.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class StateStack;

	class VELOX_API State
	{
	public:
		using ID = std::uint16_t;

		using Ptr = typename std::unique_ptr<State>;
		using Func = typename std::function<Ptr()>;

		struct VELOX_API Context // holds vital objects
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
}