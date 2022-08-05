#include "InputHandler.h"

using namespace fge;

InputHandler::InputHandler()
{
	memset(_current_button_state, false, sizeof(_current_button_state));
	memset(_previous_button_state, false, sizeof(_previous_button_state));

	memset(_current_key_state, false, sizeof(_current_key_state));
	memset(_previous_key_state, false, sizeof(_previous_key_state));

	for (unsigned short i = 0; i < sf::Mouse::ButtonCount; ++i)
		_button_bindings[i] = static_cast<sf::Mouse::Button>(i);

	for (unsigned short i = 0; i < sf::Keyboard::KeyCount; ++i)
		_key_bindings[i] = static_cast<sf::Keyboard::Key>(i);

	_scroll_delta = 0.0f;
}

InputHandler::~InputHandler()
{

}

bool InputHandler::update()
{
	for (unsigned short i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		_previous_button_state[i] = _current_button_state[i];
		_current_button_state[i] = sf::Mouse::isButtonPressed(_button_bindings[i]);
	}

	for (unsigned short i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		_previous_key_state[i] = _current_key_state[i];
		_current_key_state[i] = sf::Keyboard::isKeyPressed(_key_bindings[i]);
	}

	_scroll_delta = 0.0f;

	return true;
}
