#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace fge
{
	// translates basic input from keyboard and mouse for more extensible usage
	//
	class InputHandler
	{
	public:
		InputHandler();
		~InputHandler();

		// call at start of loop before poll event
		//
		bool update();

	public:
		inline bool get_button_pressed(sf::Mouse::Button button)
		{
			if (button < 0 || button >= sf::Mouse::ButtonCount)
				return false;

			return _current_button_state[button] && !_previous_button_state[button];
		}
		inline bool get_button_released(sf::Mouse::Button button)
		{
			if (button < 0 || button >= sf::Mouse::ButtonCount)
				return false;

			return !_current_button_state[sf::Mouse::Button::Left] && _previous_button_state[sf::Mouse::Button::Left];
		}
		inline bool get_button_held(sf::Mouse::Button button)
		{
			if (button < 0 || button >= sf::Mouse::ButtonCount)
				return false;

			return _current_button_state[sf::Mouse::Button::Left] && _previous_button_state[sf::Mouse::Button::Left];
		}

		inline bool get_key_pressed(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return (_current_key_state[key] && !_previous_key_state[key]);
		}
		inline bool get_key_released(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return (!_current_key_state[key] && _previous_key_state[key]);
		}
		inline bool get_key_held(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return _current_key_state[key] && _previous_key_state[key];
		}

		inline bool get_key_pressed(std::string name)	  { return _key_bindings.contains(name) && get_key_pressed(_key_bindings[name]); }
		inline bool get_button_pressed(std::string name)  { return _button_bindings.contains(name) && get_button_pressed(_button_bindings[name]); }

		inline bool get_key_released(std::string name)	  { return _key_bindings.contains(name) && get_key_released(_key_bindings[name]); }
		inline bool get_button_released(std::string name) { return _button_bindings.contains(name) && get_button_released(_button_bindings[name]); }

		inline bool get_key_held(std::string name)		  { return _key_bindings.contains(name) && get_key_held(_key_bindings[name]); }
		inline bool get_button_held(std::string name)	  { return _button_bindings.contains(name) && get_button_held(_button_bindings[name]); }

		inline bool get_scroll_up() const	{ return _scroll_delta > 0; }
		inline bool get_scroll_down() const { return _scroll_delta < 0; }

		void set_scroll_delta(float scroll_delta)
		{
			_scroll_delta = scroll_delta;
		}

		void set_key_binding(std::string name, sf::Keyboard::Key key)
		{
			_key_bindings[name] = key;
		}
		void set_button_binding(std::string name, sf::Mouse::Button button)
		{
			_button_bindings[name] = button;
		}

	private:
		bool _current_button_state[sf::Mouse::ButtonCount];
		bool _previous_button_state[sf::Mouse::ButtonCount];

		bool _current_key_state[sf::Keyboard::KeyCount];
		bool _previous_key_state[sf::Keyboard::KeyCount];

		float _scroll_delta;

		std::unordered_map<std::string, sf::Keyboard::Key> _key_bindings;
		std::unordered_map<std::string, sf::Mouse::Button> _button_bindings;
	};
}

