#include "Window.h"

using namespace fge;

Window::Window()
	: modes(sf::VideoMode::getFullscreenModes()), view(getDefaultView())
{

}

Window::~Window()
{

}

void Window::refresh()
{
	modes = sf::VideoMode::getFullscreenModes();
}

bool Window::set_resolution(int x, int y)
{
	for (const sf::VideoMode& mode : modes)
		if (mode.size.x == x && mode.size.y == y)
		{
			view.setSize(sf::Vector2f(mode.size));
			setView(view);

			return true;
		}

	return false;
}

bool Window::initialize()
{
	if (!modes.size())
		return false;

	sf::VideoMode video_mode = modes.front();

	if (!video_mode.isValid())
		return false;

	sf::RenderWindow window(sf::VideoMode(video_mode.size, 
		sf::VideoMode::getDesktopMode().bitsPerPixel), "Physics", sf::Style::None);

	if (!window.setActive(true))
		return false;

	return true;
}
