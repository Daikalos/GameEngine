#include "Camera.h"

using namespace fge;

void Camera::update(const InputHandler& input_handler, const sf::RenderWindow& window)
{
	if (input_handler.get_button_pressed(Binding::Button::Drag))
		_dragPos = get_mouse_world_position(window);
	if (input_handler.get_button_held(Binding::Button::Drag))
		_position += (_dragPos - get_mouse_world_position(window));

	if (input_handler.get_scroll_up())
		_scale += sf::Vector2f(0.1f, 0.1f);
	if (input_handler.get_scroll_down())
		_scale -= sf::Vector2f(0.1f, 0.1f);

	set_position(_position);
	set_scale(_scale);
}

void Camera::handle_event(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Resized:
		set_letterbox_view(event.size.width, event.size.height);
		break;
	}
}

void Camera::set_letterbox_view(int width, int height)
{
	float window_ratio = width / (float)height;
	float view_ratio = getSize().x / (float)getSize().y;

	float size_x = 1.0f;
	float size_y = 1.0f;
	float pos_x = 0.0f;
	float pos_y = 0.0f;

	if (window_ratio >= view_ratio)
	{
		size_x = view_ratio / window_ratio;
		pos_x = (1.0f - size_x) / 2.0f;
	}
	else
	{
		size_y = window_ratio / view_ratio;
		pos_y = (1.0f - size_y) / 2.0f;
	}

	setViewport(sf::FloatRect(sf::Vector2f(pos_x, pos_y), sf::Vector2f(size_x, size_y)));	
}
