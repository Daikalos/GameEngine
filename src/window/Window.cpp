#include "Window.h"

using namespace fge;

Window::Window()
	: _view(getDefaultView()), _modes(sf::VideoMode::getFullscreenModes()), _fullscreen(false)
{
	
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

void Window::refresh()
{
	_modes = sf::VideoMode::getFullscreenModes();
}

bool Window::set_resolution(int x, int y)
{
	for (const sf::VideoMode& mode : _modes)
		if (mode.size.x == x && mode.size.y == y)
		{
			_view.setSize(sf::Vector2f(mode.size));
			setView(_view);

			return true;
		}

	return false;
}

bool Window::initialize()
{
	if (!_modes.size())
		return false;

	sf::VideoMode video_mode = _modes.front();

	if (!video_mode.isValid())
		return false;

	create(sf::VideoMode(video_mode.size,
		sf::VideoMode::getDesktopMode().bitsPerPixel), "Physics", sf::Style::None);

	if (!setActive(true))
		return false;

	return true;
}
