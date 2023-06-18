#include "Application.h"

using namespace vlx;

Application::Application(const std::string_view name) : m_world(name) {}

void Application::Run()
{
	LoadTextures();
	RegisterControls();
	RegisterStates();

	m_world.Run();
}

void Application::LoadTextures()
{
	auto& texture_holder = m_world.GetTextureHolder();

	auto load = texture_holder.LoadAsync(Texture::ID::IdleCursor, TEXTURE_FOLDER + "cursor.png");
	auto load2 = texture_holder.LoadAsync(Texture::ID::Square, TEXTURE_FOLDER + "square.png");
	auto load3 = texture_holder.LoadAsync(Texture::ID::Circle, TEXTURE_FOLDER + "circle.png");
	auto load4 = texture_holder.LoadAsync(Texture::ID::White, TEXTURE_FOLDER + "white.png");

	load.wait();
	load2.wait();
	load3.wait();
	load4.wait();

	m_world.GetControls().Get<MouseCursor>().SetTexture(texture_holder.Get(Texture::ID::IdleCursor));
}

void Application::RegisterStates()
{
	// add states (e.g. gameover, win, play, paused)
	StateStack& state_stack = m_world.GetStateStack();
	Camera& camera = m_world.GetCamera();

	state_stack.RegisterState<StateTest>(0);

	state_stack.Push(0);

	camera.RegisterBehavior<CameraDrag>(0);
	camera.RegisterBehavior<CameraZoom>(1);
	camera.RegisterBehavior<CameraFollow>(2);

	camera.Push(0);
	camera.Push(1);
}

void Application::RegisterControls()
{
	auto& controls = m_world.GetControls();

	controls.Get<KeyboardInput>().AddMap<bn::Key>();
	controls.Get<MouseInput>().AddMap<bn::Button>();
	controls.Get<JoystickInput>().AddMap<bn::XboxButton>();

	controls.Get<KeyboardInput>().GetMap<bn::Key>().Set(bn::Key::Left, sf::Keyboard::A);
	controls.Get<MouseInput>().GetMap<bn::Button>().Set(bn::Button::Drag, sf::Mouse::Middle);
}
