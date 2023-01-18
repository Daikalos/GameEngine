#include "Application.h"

using namespace vlx;

Application::Application(std::string_view name) :
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Windowed, sf::ContextSettings(), false, 300),
	m_camera(CameraBehavior::Context(m_window, m_controls)),
	m_state_stack(State<>::Context(m_window, m_camera, m_controls, m_texture_holder, m_font_holder)),
	m_world(m_window, m_time)
{

}

void Application::Run()
{
	//////////////////////-INITIALIZE-//////////////////////////

	EntityAdmin& entity_admin = m_world.GetEntityAdmin();
	entity_admin.RegisterComponents<>(AllTypes{});

	m_window.Initialize();

	m_camera.SetSize(sf::Vector2f(m_window.getSize()));
	m_camera.SetPosition(m_camera.GetSize() / 2.0f);

	LoadMainTextures();

	RegisterControls();
	RegisterStates();

	float accumulator = FLT_EPSILON;

	int ticks = 0;
	int death_spiral = 12; // guarantee prevention of infinite loop

	ObjectSystem& object_system = m_world.Get<ObjectSystem>();
	TransformSystem& transform_system = m_world.Get<TransformSystem>();

	Entity entity = object_system.CreateObject();
	entity.AddComponents(ObjectType{});

	entity.GetComponent<Sprite>().SetTexture(m_texture_holder.Get(Texture::ID::IdleCursor));
	entity.GetComponent<Sprite>().SetOpacity(1.0f);

	ComponentRefPtr<Transform> transform = entity.GetComponentRef<Transform>();
	transform->Get()->SetPosition({50.0f, 50.0f});

	Entity new_entity = entity.Duplicate();
	ComponentRefPtr<Transform> transform2 = new_entity.GetComponentRef<Transform>();

	Transform& test = entity_admin.GetComponent<Transform>(entity.GetID());

	auto set = new_entity.GetComponents<Object, Transform>();

	entity_admin.SetComponent<Transform>(entity.GetID(), set.Get<Transform>());

	m_world.Get<RelationSystem>().AttachInstant(entity.GetID(), new_entity.GetID());

	std::puts(std::to_string(set.Get<Object>().IsAlive).c_str());
	std::puts(std::to_string(entity.TryGetComponentRef<Sprite>().first->Get()->GetSize().x).c_str());
	std::puts(std::to_string(entity_admin.GetEntitiesWith<Sprite>().front()).c_str());

	Entity label_entity = new_entity.Duplicate();
	label_entity.AddComponent<gui::Label>(105, 105);

	gui::Label& label = label_entity.GetComponent<gui::Label>();
	label.setString("potato");

	float x_pos = 0.0f;

	while (m_window.isOpen())
	{
		m_time.Update();

		m_controls.UpdateAll(m_time, m_window.hasFocus());

		ProcessEvents();

		PreUpdate();

		Update();

		accumulator += m_time.GetRealDT();
		ticks = 0;

		while (accumulator >= m_time.GetFixedDT() && ticks++ < death_spiral)
		{
			accumulator -= m_time.GetFixedDT();
			FixedUpdate();
		}

		float interp = accumulator / m_time.GetFixedDT();
		m_time.SetInterp(interp);

		PostUpdate();

		if (m_state_stack.IsEmpty())
			m_window.close();

		if (m_controls.Get<KeyboardInput>().Pressed(sf::Keyboard::Space))
			m_controls.Get<MouseCursor>().SetIsLocked(true);

		x_pos += m_time.GetDT() * 5.0f;

		if (entity_admin.IsEntityRegistered(entity.GetID()))
			transform->Get()->SetPosition({ x_pos, 10.0f });

		transform2->Get()->SetPosition({ 0.0f, x_pos });

		m_window.setTitle(std::to_string(m_time.GetFramerate()));

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
	m_controls.Add<MouseCursor>(m_window, m_texture_holder.Get(Texture::ID::IdleCursor), false);

	m_controls.Get<KeyboardInput>().AddMap<bn::Key>();
	m_controls.Get<MouseInput>().AddMap<bn::Button>();
	m_controls.Get<JoystickInput>().AddMap<bn::XboxButton>();

	m_controls.Get<KeyboardInput>().GetMap<bn::Key>().Set(bn::Key::Left, sf::Keyboard::A);
	m_controls.Get<MouseInput>().GetMap<bn::Button>().Set(bn::Button::Drag, sf::Mouse::Middle);
}

void Application::LoadMainTextures()
{
	auto load = m_texture_holder.LoadAsync(Texture::ID::IdleCursor, TEXTURE_FOLDER + "cursor.png");
	auto load2 = m_texture_holder.LoadAsync(Texture::ID::Square, TEXTURE_FOLDER + "square.png");

	load.wait();
	load2.wait();
}
