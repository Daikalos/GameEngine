#include "Camera.h"

using namespace fge;

Camera::Camera(const Window* window)
	: _window(window), _position(sf::Vector2f(window->getSize()) / 2.0f), _scale({ 1.0f, 1.0f })
{

}

void Camera::update(const InputHandler& input_handler)
{
	if (input_handler.get_key_pressed(sf::Keyboard::Key::Space))
		set_position(sf::Vector2i(_window->getSize() / 2U));

	if (input_handler.get_button_pressed(KeyName::Drag))
		_dragPos = get_mouse_world_position();
	if (input_handler.get_button_held(KeyName::Drag))
		_position += (_dragPos - get_mouse_world_position());
}
