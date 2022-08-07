#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace fge
{
	namespace Bindings
	{
		enum Button : uint32_t
		{
			Drag,

			ButtonBindingCount
		};

		enum Key : uint32_t
		{
			Up,
			Left,
			Right,
			Down,

			KeyBindingCount
		};

		namespace Joystick
		{
			enum XboxButton : uint32_t
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

			enum XboxAxis : uint32_t
			{
				LX,
				LY,

				XboxAxisCount
			};

			enum PlaystationButton : uint32_t // not checked
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

			enum PlaystationAxis : uint32_t
			{

			};
		};
	}

	// translates basic input from keyboard and mouse for more extensible usage
	//
	class InputHandler
	{
	public:
		InputHandler();
		~InputHandler();

		// call at start of loop before poll event
		//
		void update();
		void handle_event(const sf::Event& event);

	public:
		inline bool get_button_held(const sf::Mouse::Button& button) const
		{
			return _current_button_state[button] && _previous_button_state[button];
		}
		inline bool get_button_pressed(const sf::Mouse::Button& button) const
		{
			return _current_button_state[button] && !_previous_button_state[button];
		}
		inline bool get_button_released(const sf::Mouse::Button& button) const
		{
			return !get_button_pressed(button);
		}

		inline bool get_key_held(const sf::Keyboard::Key& key) const
		{
			return _current_key_state[key] && _previous_key_state[key];
		}
		inline bool get_key_pressed(const sf::Keyboard::Key& key) const
		{
			return _current_key_state[key] && !_previous_key_state[key];
		}
		inline bool get_key_released(const sf::Keyboard::Key& key) const
		{
			return !get_key_pressed(key);
		}

		inline bool get_joystick_button_held(const uint32_t& id, const uint32_t& button) const
		{ 
			int index = button + id * sf::Joystick::ButtonCount;
			return _current_button_joystick_state[index] && _previous_button_joystick_state[index];
		}
		inline bool get_joystick_button_pressed(const uint32_t& id, const uint32_t& button) const
		{
			int index = button + id * sf::Joystick::ButtonCount;
			return _current_button_joystick_state[index] && !_previous_button_joystick_state[index];
		}
		inline bool get_joystick_button_released(const uint32_t& id, const uint32_t& button) const
		{
			return get_joystick_button_pressed(id, button);
		}

		inline bool get_button_held(const Bindings::Button& bind) const { return _button_bindings.contains(bind) && get_button_held(_button_bindings.at(bind)); }
		inline bool get_button_pressed(const Bindings::Button& bind) const { return _button_bindings.contains(bind) && get_button_pressed(_button_bindings.at(bind)); }
		inline bool get_button_released(const Bindings::Button& bind) const { return _button_bindings.contains(bind) && get_button_released(_button_bindings.at(bind)); }

		inline bool get_scroll_up() const { return _scroll_delta > 0; }
		inline bool get_scroll_down() const { return _scroll_delta < 0; }

		inline bool get_key_held(const Bindings::Key& bind) const { return _key_bindings.contains(bind) && get_key_held(_key_bindings.at(bind)); }
		inline bool get_key_pressed(const Bindings::Key& bind) const { return _key_bindings.contains(bind) && get_key_pressed(_key_bindings.at(bind)); }
		inline bool get_key_released(const Bindings::Key& bind) const { return _key_bindings.contains(bind) && get_key_released(_key_bindings.at(bind)); }

		inline bool get_joystick_button_held(const uint32_t& id, const uint32_t& name) const
		{ 
			return _joystick_bindings.contains(name) && get_joystick_button_held(id, _joystick_bindings.at(name));
		}
		inline bool get_joystick_button_pressed(const uint32_t& id, const uint32_t& name) const
		{
			return _joystick_bindings.contains(name) && get_joystick_button_pressed(id, _joystick_bindings.at(name));
		}
		inline bool get_joystick_button_released(const uint32_t& id, const uint32_t& name) const
		{
			return _joystick_bindings.contains(name) && get_joystick_button_released(id, _joystick_bindings.at(name));
		}

		void set_key_binding(const Bindings::Key& name, const sf::Keyboard::Key& key)
		{
			_key_bindings[name] = key;
		}
		void set_button_binding(const Bindings::Button& name, const sf::Mouse::Button& button)
		{
			_button_bindings[name] = button;
		}
		void set_joystick_button_binding(const uint32_t& name, const uint32_t& button)
		{
			_joystick_bindings[name] = button;
		}

		void set_keyboard_enabled(bool flag)
		{
			_keyboard_enabled = flag;
		}
		void set_mouse_enabled(bool flag)
		{
			_mouse_enabled = flag;
		}
		void set_joystick_enabled(bool flag)
		{
			_joystick_enabled = flag;
		}

	private: // VARIABLES

		//////////////////////////////////// MOUSE

		bool _current_button_state[sf::Mouse::ButtonCount];
		bool _previous_button_state[sf::Mouse::ButtonCount];

		float _scroll_delta;

		//////////////////////////////////// KEYBOARD

		bool _current_key_state[sf::Keyboard::KeyCount];
		bool _previous_key_state[sf::Keyboard::KeyCount];

		//////////////////////////////////// JOYSTICK

		std::unordered_set<uint32_t> _available_joysticks;

		bool _current_button_joystick_state[sf::Joystick::Count * sf::Joystick::ButtonCount];
		bool _previous_button_joystick_state[sf::Joystick::Count * sf::Joystick::ButtonCount];

		sf::Vector2i _joystick_axis[sf::Joystick::Count * sf::Joystick::AxisCount];

		//////////////////////////////////// MISC

		bool _keyboard_enabled;
		bool _mouse_enabled;
		bool _joystick_enabled;

		std::unordered_map<Bindings::Key, sf::Keyboard::Key> _key_bindings;
		std::unordered_map<Bindings::Button, sf::Mouse::Button> _button_bindings;
		std::unordered_map<uint32_t, uint32_t> _joystick_bindings;
	};
}

