#include "Application.h"

using namespace vlx;

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

	LoadMainTextures();

	RegisterControls();
	RegisterStates();

	m_state_stack.Push(state::ID::Test);

	////////////////////////////////////////////////////////////

	bm::Begin();

	for (int j = 0; j < 50; ++j)
	{
		EntityAdmin entity_admin;

		entity_admin.RegisterComponent<Velocity>();

		std::vector<Entity> entities;

		for (int i = 0; i < 10; ++i)
		{
			Entity& entity = entities.emplace_back(entity_admin); 
			entity.Add<Velocity>(sf::Vector2f(rnd::random(0.0f, 5.0f), 0.0f));
		}


		entity_admin.RunSystems(0, m_time);
	}

	bm::End();

	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	while (m_window.isOpen())
	{
		m_time.Update();

		m_controls.UpdateAll(m_time, m_window.hasFocus());

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
		m_time.SetInterp(interp);

		PostUpdate();

		if (m_state_stack.IsEmpty())
			m_window.close();

		//if (m_controls.Get<MouseInputBindable>().ScrollUp())
		//	m_controls.Remove<MouseCursor>();
		//if (m_controls.Get<MouseInputBindable>().ScrollDown())
		//	m_controls.Add<MouseCursor>(m_window, m_texture_holder);

		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num1))
			m_window.SetBorder(WindowBorder::Windowed);
		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num2))
			m_window.SetBorder(WindowBorder::Fullscreen);
		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num3))
			m_window.SetBorder(WindowBorder::BorderlessWindowed);

		if (m_controls.Get<KeyboardInput>().Released(sf::Keyboard::Key::Num4))
			m_window.SetMode(sf::VideoMode::getFullscreenModes().back());
		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num5))
			m_window.SetResolution(0);
		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num6))
			m_window.SetResolution(1);
		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Key::Num7))
			m_window.SetResolution(2);

		Draw();
	}
}

void Application::ProcessEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_controls.HandleEventAll(event);
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

void Application::PostUpdate()
{
	m_state_stack.PostUpdate(m_time);
	m_camera.PostUpdate(m_time);
}

void Application::Draw()
{
	m_window.clear(sf::Color::Black);
	m_window.setView(m_camera);

	m_state_stack.Draw();

	m_sprite_batch.SetSortMode(SortMode::FrontToBack);

	Sprite sprite1(m_texture_holder.Get(Texture::ID::IdleCursor));
	sprite1.SetColor(sf::Color::Red);
	Sprite sprite2(m_texture_holder.Get(Texture::ID::IdleCursor));
	sprite2.SetColor(sf::Color::Blue);
	Sprite sprite3(m_texture_holder.Get(Texture::ID::IdleCursor), -10.0f);
	sprite3.SetColor(sf::Color::Green);

	Transform transform1 = Transform();
	transform1.setPosition(sf::Vector2f(-10.0f, 0.0f));
	Transform transform2 = Transform();
	transform2.setPosition(sf::Vector2f(0.0f, -10.0f));
	Transform transform3 = Transform();
	transform3.setPosition(sf::Vector2f(-5.0f, -5.0f));

	m_sprite_batch.Batch(sprite1, transform1, 0.0f);
	m_sprite_batch.Batch(sprite2, transform2, 0.0f);
	m_sprite_batch.Batch(sprite3, transform3, 0.0f);
	m_window.draw(m_sprite_batch);
	m_sprite_batch.Clear();

	m_window.setView(m_window.getDefaultView()); // draw hud ontop of everything else

	if (m_controls.Exists<MouseCursor>())
		m_controls.Get<MouseCursor>().Draw();

	m_window.display();
}

void Application::RegisterStates()
{
	// add states (e.g. gameover, win, play, paused)
	m_state_stack.RegisterState<StateTest>(state::ID::Test);

	m_camera.RegisterBehaviour<CameraDrag>(camera::ID::Drag);
	m_camera.RegisterBehaviour<CameraZoom>(camera::ID::Zoom);

	m_camera.Push(camera::ID::Drag);
	m_camera.Push(camera::ID::Zoom);
}

void Application::RegisterControls()
{
	m_controls.Add<KeyboardInput>();
	m_controls.Add<MouseInput>();
	m_controls.Add<JoystickInput>();
	m_controls.Add<MouseCursor>(m_window, m_texture_holder.Get(Texture::ID::IdleCursor));

	m_controls.Get<KeyboardInput>().AddMap<bn::Key>();
	m_controls.Get<MouseInput>().AddMap<bn::Button>();
	m_controls.Get<JoystickInput>().AddMap<bn::XboxButton>();

	m_controls.Get<KeyboardInput>().GetMap<bn::Key>().Set(bn::Key::Left, sf::Keyboard::A);
	m_controls.Get<MouseInput>().GetMap<bn::Button>().Set(bn::Button::Drag, sf::Mouse::Middle);
}

void Application::LoadMainTextures()
{
	auto load = m_texture_holder.LoadAsync(Texture::ID::IdleCursor, TEXTURE_FOLDER + "cursor.png");

	load.wait();
}
