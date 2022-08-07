#include "Window.h"

using namespace fge;

Window::Window()
	: _modes(sf::VideoMode::getFullscreenModes()), _fullscreen(false)
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

bool Window::initialize()
{
	if (!_modes.size())
		return false;

	sf::VideoMode video_mode = _modes.front();

	if (!video_mode.isValid())
		return false;

	create(sf::VideoMode(video_mode.size,
		sf::VideoMode::getDesktopMode().bitsPerPixel), "Game Engine", sf::Style::Close);

	if (!setActive(true))
		return false;

	return true;
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
