#include <Velox/Window/Window.h>

using namespace vlx;

Window::Window(std::string name)
	: m_name(std::move(name)) { }

Window::Window(std::string name, const sf::VideoMode& mode)
	: m_name(std::move(name)), m_mode(mode) { }

Window::Window(std::string name, const sf::VideoMode& mode, const WindowBorder& border)
	: m_name(std::move(name)), m_mode(mode), m_border(border) { }

Window::Window(
	std::string name, 
	const sf::VideoMode& mode, 
	const WindowBorder& border, 
	const sf::ContextSettings& settings, 
	bool vertical_sync, 
	int frame_rate) :

	m_name			(std::move(name)),
	m_mode			(mode), 
	m_border		(border), 
	m_settings		(settings), 
	m_vertical_sync	(vertical_sync), 
	m_frame_rate	(frame_rate) 
{}

const std::vector<sf::VideoMode>& Window::GetFullscreenModes() const
{
	return sf::VideoMode::getFullscreenModes();
}

Vector2i Window::GetOrigin() const noexcept
{
	return Vector2i(getSize() / 2u);
}
const Vector2f& Window::GetRatioCmp() const noexcept
{
	return m_ratio_cmp;
}

void Window::onCreate()
{
	sf::RenderWindow::onCreate(); // must call base class

	SetFramerate(m_frame_rate);
	SetVerticalSync(m_vertical_sync);

	setMouseCursorVisible(true);
	setMouseCursorGrabbed(true);

	m_ratio_cmp = Vector2f(
		getSize().x / (float)sf::VideoMode::getDesktopMode().size.x,
		getSize().y / (float)sf::VideoMode::getDesktopMode().size.y);
}

void Window::Initialize()
{
	auto& modes = sf::VideoMode::getFullscreenModes();

	if (modes.empty())
		throw std::runtime_error("Unable to retrieve supported video modes");

	Build(m_border, modes.front(), m_settings);

	if (!setActive(true))
		throw std::runtime_error("Window could not be activated");
}

void Window::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		close();
		break;
	}
}

void Window::SetFramerate(int frame_rate)
{
	setFramerateLimit(m_vertical_sync ? 0 : frame_rate);
	m_frame_rate = frame_rate;
}

void Window::SetVerticalSync(bool flag)
{
	setFramerateLimit(flag ? 0 : m_frame_rate);
	setVerticalSyncEnabled(flag);

	m_vertical_sync = flag;
}

void Window::SetResolution(int index)
{
	auto& modes = GetFullscreenModes();

	if (index >= modes.size())
		throw std::runtime_error("Index is out of range");

	SetMode(modes[index]);
}

void Window::Build(WindowBorder border, sf::VideoMode mode, sf::ContextSettings settings)
{
	m_border = border;
	m_mode = mode;
	m_settings = settings;

	switch (border)
	{
	case WindowBorder::Windowed:
		create(sf::VideoMode(m_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel),
			m_name, sf::Style::Close, m_settings);
		break;
	case WindowBorder::Fullscreen:
		create(sf::VideoMode(m_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel), 
			m_name, sf::Style::Fullscreen, m_settings);
		break;
	case WindowBorder::BorderlessWindowed:
		create(sf::VideoMode(m_mode.size, sf::VideoMode::getDesktopMode().bitsPerPixel), 
			m_name, sf::Style::None, m_settings);
		break;
	}
}

void Window::SetBorder(WindowBorder border)
{
	if (m_border != border)
		Build(border, m_mode, m_settings);
}
void Window::SetMode(sf::VideoMode mode)
{
	if (m_mode != mode)
		Build(m_border, mode, m_settings);
}
void Window::SetSettings(sf::ContextSettings settings)
{
	Build(m_border, m_mode, settings);
}
