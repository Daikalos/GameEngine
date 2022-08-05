#include "Camera.h"

using namespace fge;

Camera::Camera(const Window* window)
	: _window(window), _position(sf::Vector2f(window->getSize()) / 2.0f), _scale({ 1.0f, 1.0f })
{
	setCenter(_position);
	setSize(sf::Vector2f(window->getSize()));
}

void Camera::update(const InputHandler& input_handler)
{
	if (input_handler.get_key_pressed(sf::Keyboard::Key::Space))
		set_position(sf::Vector2f(_window->getSize() / 2U));

	if (input_handler.get_button_pressed(ButtonName::Drag))
		_dragPos = get_mouse_world_position();
	if (input_handler.get_button_held(ButtonName::Drag))
		_position += (_dragPos - get_mouse_world_position());

	setCenter(_position);
	setSize(sf::Vector2f(_window->getSize()) * (1.0f / _scale));
}
