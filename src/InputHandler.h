#pragma once

#include <SFML/Graphics.hpp>

namespace sfpl
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
		inline bool get_left_pressed() const { return _currentButtonState[sf::Mouse::Button::Left] && !_previousButtonState[sf::Mouse::Button::Left]; }
		inline bool get_right_pressed() const { return _currentButtonState[sf::Mouse::Button::Right] && !_previousButtonState[sf::Mouse::Button::Right]; }
		inline bool get_middle_pressed() const { return _currentButtonState[sf::Mouse::Button::Middle] && !_previousButtonState[sf::Mouse::Button::Middle]; }

		inline bool get_left_released() const { return !_currentButtonState[sf::Mouse::Button::Left] && _previousButtonState[sf::Mouse::Button::Left]; }
		inline bool get_right_released() const { return !_currentButtonState[sf::Mouse::Button::Right] && _previousButtonState[sf::Mouse::Button::Right]; }
		inline bool get_middle_released() const { return !_currentButtonState[sf::Mouse::Button::Middle] && _previousButtonState[sf::Mouse::Button::Middle]; }

		inline bool get_left_held() const { return _currentButtonState[sf::Mouse::Button::Left] && _previousButtonState[sf::Mouse::Button::Left]; }
		inline bool get_right_held() const { return _currentButtonState[sf::Mouse::Button::Right] && _previousButtonState[sf::Mouse::Button::Right]; }
		inline bool get_middle_held() const { return _currentButtonState[sf::Mouse::Button::Middle] && _previousButtonState[sf::Mouse::Button::Middle]; }

		inline bool get_scroll_up() const { return _scrollDelta > 0; }
		inline bool get_scroll_down() const { return _scrollDelta < 0; }

		inline bool get_key_pressed(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return (_currentKeyState[key] && !_previousKeyState[key]);
		}

		inline bool get_key_released(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return (!_currentKeyState[key] && _previousKeyState[key]);
		}

		inline bool get_key_held(sf::Keyboard::Key key) const
		{
			if (key < 0 || key >= sf::Keyboard::Key::KeyCount)
				return false;

			return _currentKeyState[key] && _previousKeyState[key];
		}

		void set_scrollDelta(float scrollDelta)
		{
			this->_scrollDelta = scrollDelta;
		}

	private:
		bool _currentButtonState[sf::Mouse::ButtonCount];
		bool _previousButtonState[sf::Mouse::ButtonCount];

		bool _currentKeyState[sf::Keyboard::KeyCount];
		bool _previousKeyState[sf::Keyboard::KeyCount];

		float _scrollDelta;
	};
}

