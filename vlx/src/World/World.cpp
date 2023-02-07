#include <Velox/World/World.h>

using namespace vlx;

World::World(const std::string_view name) : 
	m_entity_admin(), m_systems(), m_sorted_systems(),
	m_controls(),
	m_window(name, sf::VideoMode().getDesktopMode(), WindowBorder::Windowed, sf::ContextSettings(), false, 300),
	m_camera(CameraBehavior::Context(m_window, m_controls)),
	m_textures(), m_fonts(),
	m_time(), m_state_stack(*this)
{
	m_window.Initialize();

	m_controls.Add<KeyboardInput>();
	m_controls.Add<MouseInput>();
	m_controls.Add<JoystickInput>();
	m_controls.Add<MouseCursor>(m_window);

	m_controls.Get<KeyboardInput>().AddMap<ebn::Key>();
	m_controls.Get<MouseInput>().AddMap<ebn::Button>();

	m_controls.Get<MouseInput>().GetMap<ebn::Button>().Set(ebn::Button::GUIButton, sf::Mouse::Left);

	AddSystem<ObjectSystem>(m_entity_admin,		LYR_OBJECTS);
	AddSystem<RelationSystem>(m_entity_admin,	LYR_NONE);
	AddSystem<TransformSystem>(m_entity_admin,	LYR_TRANSFORM);
	AddSystem<AnchorSystem>(m_entity_admin,		LYR_ANCHOR, m_window);
	AddSystem<RenderSystem>(m_entity_admin,		LYR_RENDERING);
	AddSystem<gui::GUISystem>(m_entity_admin,	LYR_GUI, m_camera, m_controls);
}

const ControlMap& World::GetControls() const noexcept			{ return m_controls; }
ControlMap& World::GetControls() noexcept						{ return m_controls; }

const Window& World::GetWindow() const noexcept					{ return m_window; }
Window& World::GetWindow() noexcept								{ return m_window; }

const Camera& World::GetCamera() const noexcept					{ return m_camera; }
Camera& World::GetCamera() noexcept								{ return m_camera; }

const TextureHolder& World::GetTextureHolder() const noexcept	{ return m_textures; }
TextureHolder& World::GetTextureHolder() noexcept				{ return m_textures; }

const FontHolder& World::GetFontHolder() const noexcept			{ return m_fonts; }
FontHolder& World::GetFontHolder() noexcept						{ return m_fonts; }

const Time& World::GetTime() const noexcept						{ return m_time; }
Time& World::GetTime() noexcept									{ return m_time; }

const StateStack& World::GetStateStack() const noexcept			{ return m_state_stack; }
StateStack& World::GetStateStack() noexcept						{ return m_state_stack; }

EntityAdmin& World::GetEntityAdmin() noexcept					{ return m_entity_admin; }
const EntityAdmin& World::GetEntityAdmin() const noexcept		{ return m_entity_admin; }

void World::RemoveSystem(const LayerType id)
{
	m_systems.erase(std::find_if(m_systems.begin(), m_systems.end(),
		[id](const auto& pair)
		{
			return pair.second->GetID() == id;
		}));

	m_sorted_systems.erase(id);
}

bool World::HasSystem(const LayerType id) const
{
	return m_sorted_systems.contains(id);
}

void World::Run()
{
	m_camera.SetSize(sf::Vector2f(m_window.getSize()));
	m_camera.SetPosition(m_camera.GetSize() / 2.0f);

	float accumulator = FLT_EPSILON;
	int death_spiral = 12; // guarantee prevention of infinite loop

	while (m_window.isOpen())
	{
		m_time.Update();

		m_controls.UpdateAll(m_time, m_window.hasFocus());

		ProcessEvents();

		PreUpdate();

		Update();

		accumulator += m_time.GetRealDT();

		int ticks = 0;
		while (accumulator >= m_time.GetFixedDT() && ticks++ < death_spiral)
		{
			accumulator -= m_time.GetFixedDT();
			FixedUpdate();
		}

		float interp = accumulator / m_time.GetFixedDT();
		m_time.SetInterp(interp);

		PostUpdate();

		Draw();
	}
}

void World::PreUpdate()
{
	m_state_stack.PreUpdate(m_time);
	m_camera.PreUpdate(m_time);

	for (const auto& system : m_sorted_systems)
		system.second.second->PreUpdate();
}

void World::Update()
{
	m_state_stack.Update(m_time);
	m_camera.Update(m_time);

	for (const auto& system : m_sorted_systems)
		system.second.second->Update();
}

void World::FixedUpdate()
{
	m_state_stack.FixedUpdate(m_time);
	m_camera.FixedUpdate(m_time);

	for (const auto& system : m_sorted_systems)
		system.second.second->FixedUpdate();
}

void World::PostUpdate()
{
	m_state_stack.PostUpdate(m_time);
	m_camera.PostUpdate(m_time);

	for (const auto& system : m_sorted_systems)
		system.second.second->PostUpdate();

	if (m_state_stack.IsEmpty())
		m_window.close();
}

void World::ProcessEvents()
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

void World::Draw()
{
	RenderSystem* render_system = nullptr;
	if (HasSystem<RenderSystem>())
		render_system = &GetSystem<RenderSystem>();

	m_window.clear(sf::Color::Black);
	m_window.setView(m_camera);

	if (render_system)
		render_system->Draw(m_window);

	m_state_stack.Draw();

	m_window.setView(m_window.getDefaultView()); // draw hud ontop of everything else

	if (render_system)
		render_system->DrawGUI(m_window);

	m_window.display();
}
