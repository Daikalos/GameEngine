#include "Application.h"

using namespace fge;

Application::Application(const std::string& name) : 
	m_camera(CameraBehaviour::Context(m_window, m_input_handler)), 
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Fullscreen, sf::ContextSettings(), true, 200, *m_camera),
	m_input_handler(), 
	m_texture_holder(), 
	m_state_stack(State::Context(m_window, *m_camera, m_input_handler, m_texture_holder, m_font_holder))
{
	m_input_handler.SetButtonBinding(Binds::Button::Drag, sf::Mouse::Button::Middle);
}

Application::~Application()
{

}

void Application::Run()
{
	m_window.Initialize();

	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	while (m_window.isOpen())
	{
		m_time.Update();
		m_input_handler.Update(m_time);

		ProcessEvents();

		PreUpdate();

		Update();

		ticks = 0;
		accumulator += m_time.GetDeltaTime(); // TODO: called after update may have unintended effects...

		while (accumulator >= m_time.GetFixedDeltaTime() && ticks++ < death_spiral)
		{
			accumulator -= m_time.GetFixedDeltaTime();
			FixedUpdate();
		}

		float interp = accumulator / m_time.GetFixedDeltaTime();
		PostUpdate(interp);

		//if (_state_stack.is_empty())
		//	_window.close();

		if (m_input_handler.GetKeyPressed(sf::Keyboard::Key::Num1))
			m_window.SetBorder(WindowBorder::Windowed);
		if (m_input_handler.GetKeyPressed(sf::Keyboard::Key::Num2))
			m_window.SetBorder(WindowBorder::Fullscreen);
		if (m_input_handler.GetKeyPressed(sf::Keyboard::Key::Num3))
			m_window.SetBorder(WindowBorder::BorderlessWindowed);

		if (m_input_handler.GetKeyPressed(sf::Keyboard::Key::Num4))
			m_window.SetMode(sf::VideoMode::getFullscreenModes().back());
		if (m_input_handler.GetKeyPressed(sf::Keyboard::Key::Num5))
			m_window.SetResolution(2);

		Draw();
	}
}

void Application::ProcessEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_input_handler.HandleEvent(event);
		m_window.HandleEvent(event);
		m_camera.HandleEvent(event);
		m_state_stack.HandleEvent(event);
	}
}

void Application::PreUpdate()
{
	m_state_stack.PreUpdate(m_time);
	m_camera.PreUpdate(m_time);
}

void Application::Update()
{
	m_state_stack.Update(m_time);
	m_camera.Update(m_time);
}

void Application::FixedUpdate()
{
	m_state_stack.FixedUpdate(m_time);
	m_camera.FixedUpdate(m_time);
}

void Application::PostUpdate(float interp)
{
	m_state_stack.PostUpdate(m_time, interp);
	m_camera.PostUpdate(m_time, interp);
}

void Application::Draw()
{
	m_window.clear();
	m_window.setView(m_camera);

	m_state_stack.Draw();

	m_window.display();
}

void Application::RegisterStates()
{
	// add states (e.g. gameover, win, play, paused)

}
