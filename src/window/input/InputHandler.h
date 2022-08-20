#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <unordered_set>

#include "../../utilities/NonCopyable.h"
#include "../../utilities/Time.hpp"

#define KEYBOARDMOUSE_ENABLED 1
#define JOYSTICK_ENABLED 1

#include "Binds.h"

namespace fge
{
	////////////////////////////////////////////////////////////
	// Translates basic input from keyboard, mouse, and 
	// joystick for more extensible usage
	////////////////////////////////////////////////////////////
	class InputHandler final : NonCopyable
	{
	public:
		using uint = uint32_t;

	private:
		////////////////////////////////////////////////////////////
		// Wrapper for joystick button to allow for 
		// easily converting from xbox, playstation, etc.
		////////////////////////////////////////////////////////////
		struct JoystickButton
		{
			constexpr JoystickButton(const Binds::XboxButton& xbox_button)
				: m_button(static_cast<uint>(xbox_button)) { }
			constexpr JoystickButton(const Binds::PlaystationButton& playstation_button)
				: m_button(static_cast<uint>(playstation_button)) { }

			bool operator==(const JoystickButton& other) const { return m_button == other.m_button; }
			struct Hash { uint32_t operator()(const JoystickButton& joystick_button) const { return joystick_button.m_button; } };

			const uint m_button;
		};

		struct JoystickAxis
		{
			constexpr JoystickAxis(const Binds::XboxButton& xbox_axis)
				: m_axis(static_cast<uint>(xbox_axis)) { }
			constexpr JoystickAxis(const Binds::PlaystationButton& playstation_axis)
				: m_axis(static_cast<uint>(playstation_axis)) { }

			bool operator==(const JoystickAxis& other) const { return m_axis == other.m_axis; }
			struct Hash { uint32_t operator()(const JoystickAxis& joystick_axis) const { return joystick_axis.m_axis; } };

			const uint m_axis;
		};

	public:
		InputHandler();

		void HandleEvent(const sf::Event& event);

		////////////////////////////////////////////////////////////
		// call at start of loop before poll event
		////////////////////////////////////////////////////////////
		void Update(const Time& time);

	public:
#if KEYBOARDMOUSE_ENABLED
		bool GetScrollUp() const noexcept;
		bool GetScrollDown() const noexcept;

		bool GetButtonHeld(const sf::Mouse::Button& button) const;
		bool GetButtonPressed(const sf::Mouse::Button& button) const;
		bool GetButtonReleased(const sf::Mouse::Button& button) const;

		bool GetButtonHeld(const Binds::Button& button) const;
		bool GetButtonPressed(const Binds::Button& button) const;
		bool GetButtonReleased(const Binds::Button& button) const;

		bool GetKeyHeld(const sf::Keyboard::Key& key) const;
		bool GetKeyPressed(const sf::Keyboard::Key& key) const;
		bool GetKeyReleased(const sf::Keyboard::Key& key) const;

		bool GetKeyHeld(const Binds::Key& key) const;
		bool GetKeyPressed(const Binds::Key& key) const;
		bool GetKeyReleased(const Binds::Key& key) const;

		void SetButtonBinding(const Binds::Button& name, const sf::Mouse::Button& button);
		void SetKeyBinding(const Binds::Key& name, const sf::Keyboard::Key& key);

		void RemoveButtonBinding(const Binds::Button& name);
		void RemoveKeyBinding(const Binds::Key& name);
#endif
#if JOYSTICK_ENABLED
		bool GetJoystickButtonHeld(const uint& id, const uint& button) const;
		bool GetJoystickButtonPressed(const uint& id, const uint& button) const;
		bool GetJoystickButtonReleased(const uint& id, const uint& button) const;

		bool GetJoystickButtonHeld(const uint& id, const JoystickButton& name) const;
		bool GetJoystickButtonPressed(const uint& id, const JoystickButton& name) const;
		bool GetJoystickButtonReleased(const uint& id, const JoystickButton& name) const;

		float GetJoystickAxis(const uint& id, const uint& axis) const;
		float GetJoystickAxis(const uint& id, const JoystickAxis& name) const;

		void SetJoystickButtonBinding(const JoystickButton& name, const uint& button);
		void SetJoystickAxisBinding(const JoystickAxis& name, const uint& axis);

		void RemoveJoystickButtonBinding(const JoystickButton& name);
		void RemoveJoystickAxisBinding(const JoystickAxis& name);
#endif
		void SetKeyboardEnabled(bool flag) noexcept;
		void SetMouseEnabled(bool flag) noexcept;
		void SetJoystickEnabled(bool flag) noexcept;

	private:
		bool	m_keyboard_enabled;
		bool	m_mouse_enabled;
		bool	m_joystick_enabled;

		float	m_held_threshold; // threshold before button/key is considered held

#if KEYBOARDMOUSE_ENABLED
		float	m_scroll_delta;

		bool	m_current_button_state[sf::Mouse::ButtonCount];
		bool	m_previous_button_state[sf::Mouse::ButtonCount];
		float	m_button_held_timer[sf::Mouse::ButtonCount];

		bool	m_current_key_state[sf::Keyboard::KeyCount];
		bool	m_previous_key_state[sf::Keyboard::KeyCount];
		float	m_key_held_timer[sf::Keyboard::KeyCount];

		std::unordered_map<Binds::Key, sf::Keyboard::Key>		m_key_bindings;		// bindings for keys
		std::unordered_map<Binds::Button, sf::Mouse::Button>	m_button_bindings;	// bindings for buttons
#endif

#if JOYSTICK_ENABLED
		std::unordered_set<uint> m_available_joysticks; // list of indices of the currently available joysticks

		bool	m_current_button_joystick_state[sf::Joystick::Count * sf::Joystick::ButtonCount];
		bool	m_previous_button_joystick_state[sf::Joystick::Count * sf::Joystick::ButtonCount];
		float	m_joystick_button_held_timer[sf::Joystick::Count * sf::Joystick::ButtonCount];

		float	m_joystick_axis[sf::Joystick::Count * sf::Joystick::AxisCount];

		std::unordered_map<JoystickButton, uint, JoystickButton::Hash>	m_joystick_button_bindings;
		std::unordered_map<JoystickAxis, uint, JoystickAxis::Hash>		m_joystick_axis_bindings;
#endif
	};
}

