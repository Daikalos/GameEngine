#include "Application.h"

using namespace fge;

Application::Application()
	: _window(), _camera(&_window), _input_handler(), _resource_manager(), 
	_state_stack(State::Context(_window, _input_handler, _resource_manager))
{

}

Application::~Application()
{

}

void Application::run()
{
	sf::Clock clock;
	float dt = FLT_EPSILON;

	float dt_per_frame = 1.0f / 60.0f;
	float accumulator = FLT_EPSILON;

	while (_window.isOpen())
	{
		dt = std::fminf(clock.restart().asSeconds(), 0.075f);
		accumulator += dt;

		process_input();

		while (accumulator >= dt_per_frame)
		{
			update(dt_per_frame);
			accumulator -= dt_per_frame;

			if (_state_stack.is_empty())
				_window.close();
		}

		draw();
	}
}

void Application::process_input()
{
	_input_handler.update();

	sf::Event event;
	while (_window.pollEvent(event))
	{
		_state_stack.handle_event(event);

		switch (event.type)
		{
		case sf::Event::Closed:
			_window.close();
			break;
		case sf::Event::Resized:

			break;
		case sf::Event::MouseWheelScrolled:
			_input_handler.set_scroll_delta(event.mouseWheelScroll.delta);
			break;
		}
	}

	_camera.update(_input_handler);
}

void Application::update(const float& dt)
{
	_state_stack.update(dt);
}

void Application::draw()
{
	_window.clear();
	_window.setView(_camera);

	_state_stack.draw();

	_window.display();
}

void Application::register_states()
{

}
