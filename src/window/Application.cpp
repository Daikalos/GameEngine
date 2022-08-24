#include "Application.h"

using namespace fge;

Application::Application(std::string_view name) :
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Windowed, sf::ContextSettings(), false, 300),
	m_camera(CameraBehaviour::Context(m_window, m_controls)),
	m_state_stack(State::Context(m_window, m_camera, m_controls, m_texture_holder, m_font_holder))
{

}

Application::~Application()
{

}

void Application::Run()
{
	//////////////////////-INITIALIZE-//////////////////////////

	m_window.Initialize();

	m_camera.SetSize(sf::Vector2f(m_window.getSize()));
	m_camera.SetPosition(m_camera.GetSize() / 2.0f);

	Registersts();
	RegisterControls();
	LoadMainTextures();

	m_state_stack.Push(sts::ID::Test);

	////////////////////////////////////////////////////////////

	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	while (m_window.isOpen())
	{
		m_time.Update();
		m_controls.Update(m_time);

		ProcessEvents();

		PreUpdate();

		Update();

		accumulator += m_time.GetRealDeltaTime();
		ticks = 0;

		while (accumulator >= m_time.GetFixedDeltaTime() && ticks++ < death_spiral)
		{
			accumulator -= m_time.GetFixedDeltaTime();
			FixedUpdate();
		}

		float interp = accumulator / m_time.GetFixedDeltaTime();
		PostUpdate(interp);

		if (m_state_stack.IsEmpty())
			m_window.close();

		if (m_controls.Get<DefMouse>().ScrollUp())
			std::cout << "a";
		if (m_controls.Get<DefMouse>().ScrollDown())
			std::cout << "b";

		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num1))
			m_window.SetBorder(WindowBorder::Windowed);
		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num2))
			m_window.SetBorder(WindowBorder::Fullscreen);
		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num3))
			m_window.SetBorder(WindowBorder::BorderlessWindowed);

		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num4))
			m_window.SetMode(sf::VideoMode::getFullscreenModes().back());
		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num5))
			m_window.SetResolution(0);
		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num6))
			m_window.SetResolution(1);
		if (m_controls.Get<DefKeyboard>().Pressed(sf::Keyboard::Key::Num7))
			m_window.SetResolution(2);

		Draw();
	}
}

void Application::ProcessEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_controls.HandleEvent(event);
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
	m_window.clear(sf::Color::Black);
	m_window.setView(m_window.getDefaultView());

	m_controls.Get<DefMouse>().Draw(m_window); // draw cursor

	m_window.setView(m_camera);

	m_state_stack.Draw();

	m_window.display();
}

void Application::Registersts()
{
	// add states (e.g. gameover, win, play, paused)
	m_state_stack.RegisterState<StateTest>(sts::ID::Test);
}

void Application::RegisterControls()
{
	m_controls.Add<DefKeyboard>();
	m_controls.Add<DefMouse>(m_window, m_camera);
	m_controls.Add<XboxHandler>();

	m_controls.Get<DefMouse>().SetBinding(bn::Button::Drag, sf::Mouse::Button::Middle);
}

void Application::LoadMainTextures()
{
	m_texture_holder.Load(Texture::ID::Cursor, TEXTURE_FOLDER + "cursor.png");

	m_controls.Get<DefMouse>().SetCursor(m_texture_holder.Get(Texture::ID::Cursor));
}
