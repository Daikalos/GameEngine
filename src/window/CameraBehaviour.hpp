#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"
#include "../window/input/Controls.hpp"
#include "Window.h"
#include "Cameras.h"

namespace fge
{
	class Camera;

	////////////////////////////////////////////////////////////
	// Behaviour for the camera, e.g., attach, dragging, 
	// lerp, shake, letterboxview, etc. The idea for Camera
	// Behaviour is to allow for multiple types of behaviours
	// or effects for the camera that can be easily added or 
	// removed
	////////////////////////////////////////////////////////////
	class CameraBehaviour : private NonCopyable
	{
	public:
		using Ptr = typename std::unique_ptr<CameraBehaviour>;
		using Func = typename std::function<Ptr()>;

		struct Context // holds vital objects
		{
			Context(const Window& window, const Controls& controls)
				: window(&window), controls(&controls) { }

			const Window*	window;
			const Controls*	controls;
		};

		struct OnCreateArgs { }; // TODO: maybe pass structs as args instead??

	protected:
		Camera& GetCamera() const { return *m_camera; }
		const Context& GetContext() const { return m_context; }

	public:
		CameraBehaviour(cm::ID id, Camera& camera, Context context) :
			m_id(id), m_camera(&camera), m_context(context) { }
		virtual ~CameraBehaviour() = default;

		cm::ID GetId() const noexcept { return m_id; }

		////////////////////////////////////////////////////////////
		// OnActivate is called whenever the behaviour is put as 
		// last in the stack
		////////////////////////////////////////////////////////////
		virtual void OnActivate() {}

		////////////////////////////////////////////////////////////
		// OnDestroy is called when the behaviour is removed from
		// the stack
		////////////////////////////////////////////////////////////
		virtual void OnDestroy() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(const Time& time)				{ return true; }
		virtual bool Update(const Time& time) = 0;
		virtual bool FixedUpdate(const Time& time)				{ return true; }
		virtual bool PostUpdate(const Time& time, float interp) { return true; }

	private:
		cm::ID		m_id;
		Camera* const	m_camera;
		Context			m_context;
	};
}