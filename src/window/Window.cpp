#include "Window.h"

using namespace fge;

Window::Window(std::string name, sf::VideoMode mode, WindowBorder window_border, sf::ContextSettings settings, bool vertical_sync, int frame_rate)
	: _name(name), _mode(mode), _window_border(window_border), _settings(settings), _vertical_sync(vertical_sync), _frame_rate(frame_rate)
{
	std::vector<sf::VideoMode> modes = get_modes();

	if (!modes.size())
		throw std::runtime_error("unable to retrieve supported video modes");

	_mode = modes.front();
}

Window::~Window()
{

}

void Window::onCreate()
{

}

void Window::onResize()
{

}

void Window::initialize()
{
	create_window(_window_border, _mode, _settings);

	set_framerate(_frame_rate);
	set_vertical_sync(_vertical_sync);

	if (!setActive(true))
		throw std::runtime_error("window could not be activated");

	_is_active = true;
}

void Window::handle_event(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		close();
		break;
	}
}

void Window::set_framerate(int frame_rate)
{
	_frame_rate = frame_rate;
	setFramerateLimit(_vertical_sync ? 0 : _frame_rate);
}

void Window::set_vertical_sync(bool flag)
{
	setFramerateLimit(flag ? 0 : _frame_rate);
	setVerticalSyncEnabled(flag);
}

void Window::create_window(WindowBorder window_border, sf::VideoMode mode, sf::ContextSettings settings)
{
	_window_border = window_border;
	_mode = mode;
	_settings = settings;

	switch (window_border)
	{
	case WindowBorder::Windowed:
		create(sf::VideoMode(_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel), _name, sf::Style::Close, _settings);
		break;
	case WindowBorder::Fullscreen:
		create(sf::VideoMode(_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel), _name, sf::Style::Fullscreen, _settings);
		break;
	case WindowBorder::BorderlessWindowed:
		create(sf::VideoMode(_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel), _name, sf::Style::None, _settings);
		break;
	}
}

void Window::create_window(WindowBorder window_border)
{
	if (_window_border != window_border)
		create_window(window_border, _mode, _settings);
}
void Window::create_window(sf::VideoMode mode)
{
	if (_mode != mode)
		create_window(_window_border, mode, _settings);
}
void Window::create_window(sf::ContextSettings settings)
{
	create_window(_window_border, _mode, settings);
}

void Window::set_cursor_state(bool flag)
{
	setMouseCursorVisible(flag);
	setMouseCursorGrabbed(!flag);
}
