#pragma once

#include <memory>
#include <functional>

#include <Velox/Utility/NonCopyable.h>
#include <Velox/System/Time.h>
#include <Velox/Input.hpp>
#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "Window.h"

namespace vlx
{
	class Camera;

	/// Behaviour for the camera, e.g., attach, dragging, lerp, shake, letterboxview, etc. 
	/// The idea for CameraBehaviour is to allow for multiple types of behaviours or effects 
	/// for the camera that can be easily added or removed
	/// 
	class VELOX_API CameraBehavior : private NonCopyable
	{
	public:
		using ID = uint16;

		using Ptr = typename std::unique_ptr<CameraBehavior>;
		using Func = typename std::function<Ptr()>;

		struct VELOX_API Context // holds vital objects
		{
			Context(const Window& window, const ControlMap& controls);

			const Window*		window;
			const ControlMap*	controls;
		};

	public:
		CameraBehavior(const ID id, Camera& camera, Context context);
		virtual ~CameraBehavior() = default;

		NODISC const ID& GetID() const noexcept;

	protected:
		NODISC Camera& GetCamera() const;
		NODISC const Context& GetContext() const;

	public:
		virtual void OnCreate(const std::vector<std::byte>& data) {}

		/// OnActivate is called whenever the behaviour is put as last in the stack
		///
		virtual void OnActivate() {}

		/// OnDestroy is called when the behaviour is removed from the stack
		///
		virtual void OnDestroy() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool Start(const Time& time)		{ return true; }
		virtual bool PreUpdate(const Time& time)	{ return true; }
		virtual bool Update(const Time& time) = 0;
		virtual bool FixedUpdate(const Time& time)	{ return true; }
		virtual bool PostUpdate(const Time& time)	{ return true; }

	private:
		ID				m_id;
		Camera* const	m_camera;
		Context			m_context;
	};
}