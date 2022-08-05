#include <SFML/Graphics.hpp>

#include "window/Window.h"
#include "window/Camera.h"
#include "window/InputHandler.h"
#include "graphics/ResourceManager.h"

using namespace fge;

int main()
{
	Window window;
	if (!window.initialize())
		return false;

	InputHandler input_handler;

	input_handler.set_button_binding("drag", sf::Mouse::Button::Middle);

	ResourceManager resource_manager;

	Camera camera(&window);
	
	sf::Clock clock;
	float physics_dt = 1.0f / 90.0f;
	float dt = FLT_EPSILON;
	float accumulator = FLT_EPSILON;

	sf::Vector2i mouse_pos;

	bool isRunning = true;
	while (isRunning)
	{
		dt = std::fminf(clock.restart().asSeconds(), 0.075f);
		accumulator += dt;

		input_handler.update();

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized:
					window.set_resolution(event.size.width, event.size.height);
					break;
				case sf::Event::MouseWheelScrolled:
					input_handler.set_scroll_delta(event.mouseWheelScroll.delta);
					break;
			}
		}

		camera.update(input_handler);

		while (accumulator >= physics_dt)
		{


			accumulator -= physics_dt;
		}

		window.clear();



		window.display();
	}

	return 0;
}