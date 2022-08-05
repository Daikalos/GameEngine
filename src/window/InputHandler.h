#pragma once

#include <SFML/Graphics.hpp>

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
		inline bool get_left_pressed() const { return _current_button_state[sf::Mouse::Button::Left] && !_previous_button_state[sf::Mouse::Button::Left]; }
		inline bool get_right_pressed() const { return _current_button_state[sf::Mouse::Button::Right] && !_previous_button_state[sf::Mouse::Button::Right]; }
		inline bool get_middle_pressed() const { return _current_button_state[sf::Mouse::Button::Middle] && !_previous_button_state[sf::Mouse::Button::Middle]; }

		inline bool get_left_released() const { return !_current_button_state[sf::Mouse::Button::Left] && _previous_button_state[sf::Mouse::Button::Left]; }
		inline bool get_right_released() const { return !_current_button_state[sf::Mouse::Button::Right] && _previous_button_state[sf::Mouse::Button::Right]; }
		inline bool get_middle_released() const { return !_current_button_state[sf::Mouse::Button::Middle] && _previous_button_state[sf::Mouse::Button::Middle]; }

		inline bool get_left_held() const { return _current_button_state[sf::Mouse::Button::Left] && _previous_button_state[sf::Mouse::Button::Left]; }
		inline bool get_right_held() const { return _current_button_state[sf::Mouse::Button::Right] && _previous_button_state[sf::Mouse::Button::Right]; }
		inline bool get_middle_held() const { return _current_button_state[sf::Mouse::Button::Middle] && _previous_button_state[sf::Mouse::Button::Middle]; }

		inline bool get_scroll_up() const { return _scroll_delta > 0; }
		inline bool get_scroll_down() const { return _scroll_delta < 0; }

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

		void set_scroll_delta(float scroll_delta)
		{
			_scroll_delta = scroll_delta;
		}

	private:
		bool _current_button_state[sf::Mouse::ButtonCount];
		bool _previous_button_state[sf::Mouse::ButtonCount];

		bool _current_key_state[sf::Keyboard::KeyCount];
		bool _previous_key_state[sf::Keyboard::KeyCount];

		sf::Mouse::Button _button_bindings[sf::Mouse::ButtonCount];
		sf::Keyboard::Key _key_bindings[sf::Keyboard::KeyCount];

		float _scroll_delta;
	};
}

