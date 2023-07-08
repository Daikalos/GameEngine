#include "Application.h"

using namespace vlx;

Application::Application(std::string name) 
	: m_world(std::move(name)), 

	m_keyboard(m_world.GetInputs().Keyboard()), 
	m_mouse(m_world.GetInputs().Mouse()) { }

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

	auto load = texture_holder.AcquireAsync(Texture::ID::IdleCursor, 
		FromFile<sf::Texture>(TEXTURE_FOLDER + "cursor.png"));

	auto load2 = texture_holder.AcquireAsync(Texture::ID::Square, 
		FromFile<sf::Texture>(TEXTURE_FOLDER + "square.png"));

	auto load3 = texture_holder.AcquireAsync(Texture::ID::Circle, 
		FromFile<sf::Texture>(TEXTURE_FOLDER + "circle.png"));

	auto load4 = texture_holder.AcquireAsync(Texture::ID::White, 
		FromFile<sf::Texture>(TEXTURE_FOLDER + "white.png"));

	load.wait();
	load2.wait();
	load3.wait();
	load4.wait();

	m_world.GetInputs().Cursor().SetTexture(texture_holder.Get(Texture::ID::IdleCursor));
}

void Application::RegisterStates()
{
	// add states (e.g. gameover, win, play, paused)
	StateStack& state_stack = m_world.GetStateStack();
	Camera& camera = m_world.GetCamera();

	state_stack.RegisterState<StateTest>(0);

	state_stack.Push(0);

	camera.RegisterBehavior<CameraDrag>(0, m_mouse);
	camera.RegisterBehavior<CameraZoom>(1);
	camera.RegisterBehavior<CameraFollow>(2);

	camera.Push(0);
	camera.Push(1);
}

void Application::RegisterControls()
{
	m_keyboard.Set(bn::Key::Left, sf::Keyboard::A);
	m_mouse.Set(bn::Button::Drag, sf::Mouse::Middle);
}
