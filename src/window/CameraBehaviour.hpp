#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"
#include "input/InputHandler.h"
#include "Window.h"
#include "Cameras.h"

namespace fge
{
	class Camera;

	////////////////////////////////////////////////////////////
	// Behaviour for the camera, e.g., attach, dragging, 
	// lerp, shake, letterboxview, etc. The idea for Camera
	// Behaviour is to allow for multiple types of behaviours
	// for the camera that can be easily altered
	////////////////////////////////////////////////////////////
	class CameraBehaviour : private NonCopyable
	{
	public:
		using Ptr = typename std::unique_ptr<CameraBehaviour>;
		using Func = typename std::function<Ptr()>;

		struct Context // holds vital objects
		{
			Context(const Window& window, const InputHandler& input_handler)
				: window(&window), input_handler(&input_handler) { }

			const Window* 			window;
			const InputHandler* 	input_handler;
		};

	protected:
		Camera& GetCamera() const { return *m_camera; }
		const Context& GetContext() const { return m_context; }

	public:
		CameraBehaviour(Cameras::ID id, Camera& camera, Context context) :
			m_id(id), m_camera(&camera), m_context(context) { }

		virtual ~CameraBehaviour() = default;

		Cameras::ID GetId() const noexcept { return m_id; }

		virtual void OnActivate() {}
		virtual void OnDestroy() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(const Time& time)				{ return true; }
		virtual bool Update(const Time& time) = 0;
		virtual bool FixedUpdate(const Time& time)				{ return true; }
		virtual bool PostUpdate(const Time& time, float interp) { return true; }

	private:
		Cameras::ID		m_id;
		Camera* const	m_camera;
		Context			m_context;
	};
}