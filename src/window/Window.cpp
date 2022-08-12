#include "Window.h"

using namespace fge;

Window::Window(std::string name, sf::VideoMode mode, WindowBorder window_border, sf::ContextSettings settings, bool vertical_sync, int frame_rate, Camera& camera)
	: _name(name), _mode(mode), _window_border(window_border), _settings(settings), _vertical_sync(vertical_sync), _frame_rate(frame_rate), _camera(&camera)
{
	std::vector<sf::VideoMode> modes = get_modes();

	if (!modes.size())
		throw std::runtime_error("unable to retrieve supported video modes");

	_mode = modes.front();
}

Window::~Window()
{

}

void Window::initialize()
{
	build(_window_border, _mode, _settings);

	set_framerate(_frame_rate);
	set_vertical_sync(_vertical_sync);

	if (!setActive(true))
		throw std::runtime_error("window could not be activated");

	_camera->set_size(sf::Vector2f(_mode.size));
	_camera->set_position(_camera->get_size() / 2.0f);
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

void Window::set_resolution(int index)
{
	std::vector<sf::VideoMode> modes = get_modes();

	if (index >= modes.size())
		throw std::runtime_error("index is out of range");

	set_mode(modes[index]);
}

void Window::build(WindowBorder window_border, sf::VideoMode mode, sf::ContextSettings settings)
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

void Window::set_border(WindowBorder window_border)
{
	if (_window_border != window_border)
		build(window_border, _mode, _settings);
}
void Window::set_mode(sf::VideoMode mode)
{
	if (_mode != mode)
	{
		build(_window_border, mode, _settings);
		_camera->set_size(sf::Vector2f(mode.size));
	}
}
void Window::set_settings(sf::ContextSettings settings)
{
	build(_window_border, _mode, settings);
}

void Window::set_cursor_state(bool flag)
{
	setMouseCursorVisible(flag);
	setMouseCursorGrabbed(!flag);
}

std::vector<sf::VideoMode> fge::Window::get_modes() const
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	float desktop_ratio = desktop.size.x / (float)desktop.size.y;

	std::vector<sf::VideoMode> fullscreen_modes = sf::VideoMode::getFullscreenModes();

	std::vector<sf::VideoMode> valid_modes;
	valid_modes.reserve(fullscreen_modes.size());

	for (const sf::VideoMode& mode : fullscreen_modes)
	{
		float ratio = mode.size.x / (float)mode.size.y;
		if (std::fabsf(desktop_ratio - ratio) <= FLT_EPSILON)
			valid_modes.push_back(mode);
	}

	return valid_modes;
}
