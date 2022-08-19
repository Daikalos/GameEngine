#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"

#include "input/InputHandler.h"
#include "Camera.h"

namespace fge
{
	class Camera;

	////////////////////////////////////////////////////////////
	// Behaviour for the camera, e.g., attach, dragging, 
	// lerp, shake, letterboxview, etc.
	////////////////////////////////////////////////////////////
	class CameraBehaviour : private NonCopyable
	{
	public:
		using Ptr = typename std::unique_ptr<CameraBehaviour>;
		using Func = typename std::function<Ptr()>;

		struct Context // holds vital objects
		{
			Context(Window& window, InputHandler& input_handler)
				: window(&window),  input_handler(&input_handler) { }

			const Window* const			window;
			const InputHandler* const	input_handler;
		};

	public:
		CameraBehaviour(Camera& camera, Context context) :
			m_camera(&camera), m_context(context) { }

		virtual ~CameraBehaviour() = default;

		virtual void OnActivate() {}
		virtual void OnDestroy() {}

		virtual void HandleEvent(sf::Event event) = 0;

		virtual bool PreUpdate(Time& time) { return true; }
		virtual bool Update(Time& time) = 0;
		virtual bool FixedUpdate(Time& time) { return true; }
		virtual bool PostUpdate(Time& time, float interp) { return true; }

	protected:
		Camera* const	m_camera;
		Context			m_context;
	};
}