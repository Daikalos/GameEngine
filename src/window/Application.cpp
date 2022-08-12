#include "Application.h"

using namespace fge;

Application::Application(std::string name)
	: _window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Fullscreen, sf::ContextSettings(), false, 200), 
	_camera(&_window), 
	_input_handler(), 
	_texture_holder(), 
	_state_stack(State::Context(_window, _camera, _input_handler, _texture_holder))
{

}

Application::~Application()
{

}

void Application::run()
{
	_window.initialize();

	sf::Clock clock;
	float dt = FLT_EPSILON;

	float dt_per_frame = 1.0f / 60.0f;
	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	while (_window.isOpen())
	{
		dt = std::fminf(clock.restart().asSeconds(), 0.075f);
		accumulator += dt;

		_input_handler.update(dt);

		process_input();

		_camera.update(_input_handler);

		ticks = 0;
		while (accumulator >= dt_per_frame && ticks++ < death_spiral)
		{
			accumulator -= dt_per_frame;

			update(dt_per_frame);

			if (_state_stack.is_empty())
				_window.close();
		}

		draw();
	}
}

void Application::process_input()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		_window.handle_event(event);
		_input_handler.handle_event(event);
		_state_stack.handle_event(event);
	}
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
	// add states (e.g. gameover, win, play, paused)

}
