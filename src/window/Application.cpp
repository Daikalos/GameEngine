#include "Application.h"

using namespace fge;

Application::Application(const std::string& name)
	: _camera(), 
	_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Fullscreen, sf::ContextSettings(), true, 200, _camera),
	_input_handler(), 
	_texture_holder(), 
	_state_stack(State::Context(_window, _camera, _input_handler, _texture_holder))
{
	_input_handler.set_button_binding(fge::Binding::Button::Drag, sf::Mouse::Button::Middle);
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

		_camera.update(_input_handler, _window);

		ticks = 0;
		while (accumulator >= dt_per_frame && ticks++ < death_spiral)
		{
			accumulator -= dt_per_frame;

			update(dt_per_frame);

			//if (_state_stack.is_empty())
			//	_window.close();
		}

		if (_input_handler.get_key_pressed(sf::Keyboard::Key::Num1))
			_window.set_border(WindowBorder::Windowed);
		if (_input_handler.get_key_pressed(sf::Keyboard::Key::Num2))
			_window.set_border(WindowBorder::Fullscreen);
		if (_input_handler.get_key_pressed(sf::Keyboard::Key::Num3))
			_window.set_border(WindowBorder::BorderlessWindowed);

		if (_input_handler.get_key_pressed(sf::Keyboard::Key::Num4))
			_window.set_mode(sf::VideoMode::getFullscreenModes().back());
		if (_input_handler.get_key_pressed(sf::Keyboard::Key::Num5))
			_window.set_resolution(2);

		float interp = accumulator / dt_per_frame;

		draw(interp);
	}
}

void Application::process_input()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		_input_handler.handle_event(event);
		_window.handle_event(event);
		_camera.handle_event(event);

		_state_stack.handle_event(event);
	}
}

void Application::update(float dt)
{
	_state_stack.update(dt);
}

void Application::draw(float interp)
{
	_window.clear();
	_window.setView(_camera);

	sf::CircleShape circle;
	circle.setFillColor(sf::Color::Red);
	circle.setRadius(32.0f);
	circle.setPosition(sf::Vector2f(0.0f, 0.f));

	_window.draw(circle);

	_state_stack.draw();

	_window.display();
}

void Application::register_states()
{
	// add states (e.g. gameover, win, play, paused)

}
