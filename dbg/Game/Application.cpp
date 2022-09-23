#include "Application.h"

using namespace vlx;

Application::Application(std::string_view name) :
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Windowed, sf::ContextSettings(), false, 300),
	m_camera(CameraBehavior::Context(m_window, m_controls)),
	m_state_stack(State<>::Context(m_window, m_camera, m_controls, m_texture_holder, m_font_holder))
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

	////////////////////////////////////////////////////////////

	bm::Begin();

	for (int j = 0; j < 1; ++j)
	{
		EntityAdmin entity_admin;

		entity_admin.RegisterComponent<Velocity>();
		entity_admin.RegisterComponent<Sprite>();

		std::vector<Entity> entities;

		for (int i = 0; i < 15; ++i)
		{
			Entity& entity = entities.emplace_back(entity_admin); 
			entity.Add<Velocity>(sf::Vector2f(i, 0.0f));
			entity.Add<Sprite>();
		}

		System<Velocity> s0(entity_admin, 0);
		System<Velocity, Sprite> s1(entity_admin, 1);
		System<Velocity, Sprite, Node> s2(entity_admin, 0);

		//std::puts(std::to_string(entities[2].Get<Velocity>()->velocity.x).c_str());
		
		s0.Action([](Time& time, std::span<const EntityID> entities, Velocity* velocities)
			{
				for (std::size_t i = 0; i < entities.size(); ++i)
				{
					std::puts(std::to_string(velocities[i].velocity.x).c_str());
				}
			});

		s1.Action([](Time& time, std::span<const EntityID> entities, Velocity* velocities, Sprite* sprite)
			{
				for (std::size_t i = 0; i < entities.size(); ++i)
				{
					std::puts(std::to_string(velocities[i].velocity.x).c_str());
				}
			});

		entity_admin.RunSystems(0, m_time);

		for (std::size_t i = 0; i < 1000; ++i)
		{
			std::size_t index = rnd::random(0LLU, entities.size() - 1);
			Entity& entity = entities.at(index);

			float random = rnd::random();

			if (random > 0.85f)
				entity.Add<Velocity>(sf::Vector2f(index, 0.0f));
			else if (random > 0.70f)
				entity.Remove<Velocity>();
			else if (random > 0.55f)
				entity.Add<Node>();
			else if (random > 0.40f)
				entity.Remove<Node>();
			else if (random > 0.25f)
				entity.Add<Sprite>();
			else if (random > 0.0f)
				entity.Remove<Sprite>();
		}

		std::puts("");

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
	Sprite sprite3(m_texture_holder.Get(Texture::ID::IdleCursor));
	sprite3.SetColor(sf::Color::Green);

	transform1.SetScale(sf::Vector2f(1.5, 3.0));

	if (m_controls.Get<KeyboardInput>().Held(sf::Keyboard::X))
		transform3.Rotate(sf::degrees(100.0f * m_time.GetDeltaTime()));
	if (m_controls.Get<KeyboardInput>().Held(sf::Keyboard::D))
		transform1.Move(sf::Vector2f(1, 0) * 50.0f * m_time.GetDeltaTime());
	if (m_controls.Get<KeyboardInput>().Held(sf::Keyboard::A))
		transform1.Move(sf::Vector2f(-1, 0) * 50.0f * m_time.GetDeltaTime());
	if (m_controls.Get<KeyboardInput>().Held(sf::Keyboard::S))
		transform2.Move(sf::Vector2f(0, 1) * 50.0f * m_time.GetDeltaTime());

	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Q))
		transform1.AttachChild(transform2);
	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::W))
		transform3.AttachChild(transform2);
	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::E))
		transform2.AttachChild(transform1);

	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::R))
		transform1.DetachChild(transform2);
	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::T))
		transform3.DetachChild(transform2);
	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Y))
		transform2.DetachChild(transform1);

	if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Space))
		transform3.SetPosition(sf::Vector2f());

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
	m_state_stack.RegisterState<StateTest>(0);

	m_state_stack.Push(0);

	m_camera.RegisterBehavior<CameraDrag>(0);
	m_camera.RegisterBehavior<CameraZoom>(1);

	m_camera.Push(0);
	m_camera.Push(1);
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
