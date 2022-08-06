#include "InputHandler.h"

using namespace fge;

InputHandler::InputHandler()
	: _scroll_delta(0.0f)
{
	memset(_current_button_state, false, sizeof(_current_button_state));
	memset(_previous_button_state, false, sizeof(_previous_button_state));

	memset(_current_key_state, false, sizeof(_current_key_state));
	memset(_previous_key_state, false, sizeof(_previous_key_state));
}

InputHandler::~InputHandler()
{

}

bool InputHandler::update()
{
	for (unsigned short i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		_previous_button_state[i] = _current_button_state[i];
		_current_button_state[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
	}

	for (unsigned short i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		_previous_key_state[i] = _current_key_state[i];
		_current_key_state[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}

	_scroll_delta = 0.0f;

	return true;
}
