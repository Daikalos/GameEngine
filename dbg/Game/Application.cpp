#include "Application.h"

using namespace vlx;

Application::Application(std::string_view name) :
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Windowed, sf::ContextSettings(), false, 300),
	m_camera(CameraBehavior::Context(m_window, m_controls)),
	m_state_stack(State<>::Context(m_window, m_camera, m_controls, m_texture_holder, m_font_holder)),
	m_world(m_window)
{

}

void Application::Run()
{
	//////////////////////-INITIALIZE-//////////////////////////

	EntityAdmin& entity_admin = m_world.GetEntityAdmin();
	entity_admin.RegisterComponents<Object, Transform, Sprite>();

	m_window.Initialize();

	m_camera.SetSize(sf::Vector2f(m_window.getSize()));
	m_camera.SetPosition(m_camera.GetSize() / 2.0f);

	LoadMainTextures();

	RegisterControls();
	RegisterStates();

	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	ObjectSystem& object_system = m_world.GetObjectSystem();
	TransformSystem& transform_system = m_world.GetTransformSystem();

	Entity entity = m_world.GetObjectSystem().CreateObject();
	entity.AddComponents<Object, Sprite, Transform>();
	entity.GetComponent<Sprite>().SetTexture(m_texture_holder.Get(Texture::ID::IdleCursor));
	entity.GetComponent<Sprite>().SetOpacity(1.0f);

	ComponentProxy<Transform>& transform = entity.GetComponentProxy<Transform>();
	m_world.GetTransformSystem().SetPosition(*transform, {50.0f, 50.0f});

	Entity new_entity(entity_admin, entity.Duplicate());
	ComponentProxy<Transform>& transform2 = new_entity.GetComponentProxy<Transform>();

	Transform& test = entity_admin.GetComponent<Transform>(entity.GetID());

	auto set = new_entity.GetComponents<Object, Transform>();

	entity_admin.SetComponent<Transform>(entity.GetID(), set.Get<Transform>());

	//m_world.GetTransformSystem().AttachInstant(entity.GetID(), new_entity.GetID());

	std::puts(std::to_string(set.Get<Object>().is_alive).c_str());
	std::puts(std::to_string(entity.TryGetComponentProxy<Sprite>().first->Get()->GetSize().x).c_str());
	std::puts(std::to_string(entity_admin.GetEntitiesWith<Sprite>().front()).c_str());

	float x_pos = 0.0f;

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

		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Space))
			object_system.DeleteObjectInstant(entity.GetID());

		x_pos += m_time.GetDeltaTime() * 5.0f;

		if (entity_admin.IsEntityRegistered(entity.GetID()))
			m_world.GetTransformSystem().SetPosition(*transform, { x_pos, 10.0f });

		m_world.GetTransformSystem().SetPosition(*transform2, { 0.0f, x_pos });

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
	m_world.Update();
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

	m_window.draw(m_world);

	m_state_stack.Draw();

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
