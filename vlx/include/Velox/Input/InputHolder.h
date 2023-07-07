#pragma once

#include <SFML/Window/Event.hpp>

#include <Velox/System/Time.h>
#include <Velox/Utility/NonCopyable.h>
#include <Velox/Window/Window.h>
#include <Velox/Config.hpp>

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "JoystickInput.h"
#include "MouseCursor.h"

namespace vlx
{
	///	Neat wrapper around all controls and binds
	/// 
	class VELOX_API InputHolder final : private NonCopyable
	{
	public:
		InputHolder(Window& window);

	public:
		const KeyboardInput& Keyboard() const;
		KeyboardInput& Keyboard();

		const MouseInput& Mouse() const;
		MouseInput& Mouse();

		const JoystickInput& Joystick() const;
		JoystickInput& Joystick();

		const MouseCursor& Cursor() const;
		MouseCursor& Cursor();

	public:
		void Update(const Time& time, bool focus);
		void HandleEvent(const sf::Event& event);

	private:
		KeyboardInput	m_keyboard;
		MouseInput		m_mouse;
		JoystickInput	m_joystick;
		MouseCursor		m_cursor;
	};
}