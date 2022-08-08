#include "InputHandler.h"

using namespace fge;

InputHandler::InputHandler()
	: _scroll_delta(0.0f), _mouse_enabled(true), _keyboard_enabled(true), _joystick_enabled(false)
{
	memset(_current_button_state, false, sizeof(_current_button_state));
	memset(_previous_button_state, false, sizeof(_previous_button_state));

	memset(_current_key_state, false, sizeof(_current_key_state));
	memset(_previous_key_state, false, sizeof(_previous_key_state));

	memset(_current_button_joystick_state, false, sizeof(_current_button_joystick_state));
	memset(_previous_button_joystick_state, false, sizeof(_previous_button_joystick_state));

	_available_joysticks.reserve(sf::Joystick::Count);

	for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
		if (sf::Joystick::isConnected(i))
			_available_joysticks.insert(i);
}

InputHandler::~InputHandler()
{

}

void InputHandler::update()
{
	_scroll_delta = 0.0f;

	for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		_previous_button_state[i] = _current_button_state[i];
		_current_button_state[i] = _mouse_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
	}

	for (uint32_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		_previous_key_state[i] = _current_key_state[i];
		_current_key_state[i] = _keyboard_enabled && sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}

	for (const uint32_t& i : _available_joysticks)
	{
		for (uint32_t j = 0; j < sf::Joystick::getButtonCount(i); ++j)
		{
			uint32_t k = j + i * sf::Joystick::ButtonCount;

			_previous_button_joystick_state[k] = _current_button_joystick_state[k];
			_current_button_joystick_state[k] = _joystick_enabled && sf::Joystick::isButtonPressed(i, j);
		}

		for (uint32_t j = 0; j < sf::Joystick::AxisCount; ++j)
		{
			uint32_t k = j + i * sf::Joystick::AxisCount;
			sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);

			_joystick_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) : 0.0f;
		}
	}
}

void InputHandler::handle_event(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::MouseWheelScrolled:
		_scroll_delta = event.mouseWheelScroll.delta;
		break;
	case sf::Event::JoystickConnected:
		_available_joysticks.insert(event.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
		_available_joysticks.erase(event.joystickConnect.joystickId);
		break;
	}
}
