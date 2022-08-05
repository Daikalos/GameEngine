#include <SFML/Graphics.hpp>

#include "Window.h"
#include "Camera.h"
#include "InputHandler.h"
#include "ResourceManager.h"

using namespace sfpl;

int main()
{
	Window window;
	if (!window.initialize())
		return false;

	ResourceManager resource_manager;

	Camera camera(&window);
	InputHandler input_handler;
	

	sf::Clock clock;
	float physics_dt = 1.0f / 90.0f;
	float dt = FLT_EPSILON;
	float accumulator = FLT_EPSILON;

	sf::Vector2i mouse_pos;

	while (window.isOpen())
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
					input_handler.set_scrollDelta(event.mouseWheelScroll.delta);
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