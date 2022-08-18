#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <unordered_set>
#include <string>

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"

#define KEYBOARDMOUSE_ENABLED 1
#define JOYSTICK_ENABLED 1

namespace fge
{
	namespace Binds
	{
#if KEYBOARDMOUSE_ENABLED
		enum class Button : uint32_t
		{
			Drag,

			ButtonBindingCount
		};

		enum class Key : uint32_t
		{
			Up,
			Left,
			Right,
			Down,

			KeyBindingCount
		};
#endif

#if JOYSTICK_ENABLED
		enum class XboxButton : uint32_t
		{
			A,
			B,
			X,
			Y,
			LB,
			RB,
			Back,
			Start,
			L,
			R,

			XboxButtonCount
		};
		enum class PlaystationButton : uint32_t // not checked
		{
			A,
			B,
			X,
			Y,
			LB,
			RB,
			Back,
			Start,
			L,
			R,

			Playstation
		};

		enum class XboxAxis : uint32_t
		{
			LX,
			LY,

			XboxAxisCount
		};
		enum class PlaystationAxis : uint32_t
		{

		};
#endif
	}

	// translates basic input from keyboard and mouse for more extensible usage
	//
	class InputHandler final : NonCopyable
	{
	public:
		using uint = uint32_t;

	private:
		struct JoystickButton // wrapper for joystick button to allow for easily converting from xbox, playstation, etc.
		{
			constexpr JoystickButton(const Binds::XboxButton& xbox_button)
				: m_button(static_cast<uint>(xbox_button)) { }
			constexpr JoystickButton(const Binds::PlaystationButton& playstation_button)
				: m_button(static_cast<uint>(playstation_button)) { }

			bool operator==(const JoystickButton& other) const { return m_button == other.m_button; }
			struct Hash { uint32_t operator()(const JoystickButton& joystick_button) const { return joystick_button.m_button; } };

			const uint m_button;
		};

		struct JoystickAxis // wrapper for joystick axis to allow for easily converting from xbox, playstation, etc.
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
		~InputHandler() = default;

		///////////////////////////////////////////////
		// call at start of loop before poll event
		///////////////////////////////////////////////
		void Update(const Time& time);
		void HandleEvent(const sf::Event& event);

	public:
#if KEYBOARDMOUSE_ENABLED
		inline bool GetScrollUp() const		{ return m_scroll_delta > 0; }
		inline bool GetScrollDown() const	{ return m_scroll_delta < 0; }

		inline bool GetButtonHeld(const sf::Mouse::Button& button) const		
		{ 
			return m_current_button_state[button] && m_button_held_timer[button] >= m_held_threshold;
		}
		inline bool GetButtonPressed(const sf::Mouse::Button& button) const		
		{ 
			return m_current_button_state[button] && !m_previous_button_state[button]; 
		}
		inline bool GetButtonReleased(const sf::Mouse::Button& button) const	
		{
			return !GetButtonPressed(button); 
		}

		inline bool GetButtonHeld(const Binds::Button& bind) const 
		{ 
			assert(m_button_bindings.contains(bind)); // should exist in the first place if trying to get
			return GetButtonHeld(m_button_bindings.at(bind)); 
		}
		inline bool GetButtonPressed(const Binds::Button& bind) const 
		{ 
			assert(m_button_bindings.contains(bind));
			return GetButtonPressed(m_button_bindings.at(bind)); 
		}
		inline bool GetButtonReleased(const Binds::Button& bind) const 
		{ 
			assert(m_button_bindings.contains(bind));
			return GetButtonReleased(m_button_bindings.at(bind)); 
		}

		inline bool GetKeyHeld(const sf::Keyboard::Key& key) const 
		{ 
			return m_current_key_state[key] && m_key_held_timer[key] >= m_held_threshold; 
		}
		inline bool GetKeyPressed(const sf::Keyboard::Key& key) const 
		{ 
			return m_current_key_state[key] && !m_previous_key_state[key]; 
		}
		inline bool GetKeyReleased(const sf::Keyboard::Key& key) const 
		{ 
			return !GetKeyPressed(key); 
		}

		inline bool GetKeyHeld(const Binds::Key& bind) const 
		{ 
			assert(m_key_bindings.contains(bind));
			return GetKeyHeld(m_key_bindings.at(bind));
		}
		inline bool GetKeyPressed(const Binds::Key& bind) const 
		{ 
			assert(m_key_bindings.contains(bind));
			return GetKeyPressed(m_key_bindings.at(bind)); 
		}
		inline bool GetKeyReleased(const Binds::Key& bind) const 
		{ 
			assert(m_key_bindings.contains(bind));
			return GetKeyReleased(m_key_bindings.at(bind));
		}

		void SetButtonBinding(const Binds::Button& name, const sf::Mouse::Button& button) 
		{ 
			m_button_bindings[name] = button; 
		}
		void SetKeyBinding(const Binds::Key& name, const sf::Keyboard::Key& key) 
		{ 
			m_key_bindings[name] = key; 
		}

		void RemoveJoystickButtonBinding(const JoystickButton& name)
		{
			assert(m_joystick_button_bindings.contains(name)); // should not be removed if it does not exist in the first place
			m_joystick_button_bindings.erase(name);
		}
		void RemoveJoystickAxisBinding(const JoystickAxis& name)
		{
			assert(m_joystick_axis_bindings.contains(name));
			m_joystick_axis_bindings.erase(name);
		}
#endif

#if JOYSTICK_ENABLED
		inline bool GetJoystickButtonHeld(const uint& id, const uint& button) const
		{
			int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_joystick_state[index] && m_joystick_button_held_timer[index] >= m_held_threshold;;
		}
		inline bool GetJoystickButtonPressed(const uint& id, const uint& button) const
		{
			int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_joystick_state[index] && !m_previous_button_joystick_state[index];
		}
		inline bool GetJoystickButtonReleased(const uint& id, const uint& button) const
		{
			return !GetJoystickButtonPressed(id, button);
		}

		inline bool GetJoystickButtonHeld(const uint& id, const JoystickButton& name) const 
		{ 
			assert(m_joystick_button_bindings.contains(name));
			return GetJoystickButtonHeld(id, m_joystick_button_bindings.at(name)); 
		}
		inline bool GetJoystickButtonPressed(const uint& id, const JoystickButton& name) const 
		{ 
			assert(m_joystick_button_bindings.contains(name));
			return GetJoystickButtonPressed(id, m_joystick_button_bindings.at(name)); 
		}
		inline bool GetJoystickButtonReleased(const uint& id, const JoystickButton& name) const 
		{ 
			assert(m_joystick_button_bindings.contains(name));
			return GetJoystickButtonReleased(id, m_joystick_button_bindings.at(name)); 
		}

		inline float GetJoystickAxis(const uint& id, const uint& axis) const
		{
			return m_joystick_axis[axis + id * sf::Joystick::AxisCount];
		}

		inline float GetJoystickAxis(const uint& id, const JoystickAxis& name) const
		{
			assert(m_joystick_axis_bindings.contains(name));
			return GetJoystickAxis(id, m_joystick_axis_bindings.at(name));
		}

		void SetJoystickButtonBinding(const JoystickButton& name, const uint& button) 
		{
			m_joystick_button_bindings[name] = button;
		}
		void SetJoystickAxisBinding(const JoystickAxis& name, const uint& axis) 
		{ 
			m_joystick_axis_bindings[name] = axis; 
		}

		void RemoveJoystickButtonBinding(const JoystickButton& name) 
		{ 
			assert(m_joystick_button_bindings.contains(name)); // should not be removed if it does not exist in the first place
			m_joystick_button_bindings.erase(name);
		}
		void RemoveJoystickAxisBinding(const JoystickAxis& name)
		{
			assert(m_joystick_axis_bindings.contains(name));
			m_joystick_axis_bindings.erase(name);
		}
#endif
		void SetKeyboardEnabled(bool flag)	{ m_keyboard_enabled = flag; }
		void SetMouseEnabled(bool flag)		{ m_mouse_enabled = flag; }
		void SetJoystickEnabled(bool flag)	{ m_joystick_enabled = flag; }

	private: // VARIABLES
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

