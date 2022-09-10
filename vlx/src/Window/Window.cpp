#include <Velox/Window/Window.h>

using namespace vlx;

Window::Window(
	std::string_view name, const sf::VideoMode& mode, const WindowBorder& border, const sf::ContextSettings& settings, bool vertical_sync, int frame_rate) :
	m_name(name.data()), m_mode(mode), m_border(border), m_settings(settings), m_vertical_sync(vertical_sync), m_frame_rate(frame_rate) {}

[[nodiscard]] const std::vector<sf::VideoMode>& Window::GetValidModes(bool update) const
{
	if (update || m_cached_modes.empty())
	{
		const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		float desktop_ratio = desktop.size.x / (float)desktop.size.y;

		std::vector<sf::VideoMode> fullscreen_modes = sf::VideoMode::getFullscreenModes();

		std::vector<sf::VideoMode> valid_modes;
		valid_modes.reserve(fullscreen_modes.size());

		for (const sf::VideoMode& mode : fullscreen_modes)
		{
			float ratio = mode.size.x / (float)mode.size.y;
			if (std::fabsf(desktop_ratio - ratio) <= FLT_EPSILON)
				valid_modes.push_back(mode);
		}

		m_cached_modes = std::move(valid_modes);
	}

	return m_cached_modes;
}

[[nodiscard]] constexpr sf::Vector2i Window::GetOrigin() const noexcept
{
	return sf::Vector2i(getSize() / 2u);
}
[[nodiscard]] constexpr sf::Vector2f Window::GetRatioCmp() const noexcept
{
	return m_ratio_cmp;
}

void Window::SetCursorState(bool flag)
{
	setMouseCursorVisible(flag);
	setMouseCursorGrabbed(!flag);
}

void Window::onCreate()
{
	sf::RenderWindow::onCreate(); // must call base class

	SetFramerate(m_frame_rate);
	SetVerticalSync(m_vertical_sync);

	SetCursorState(false); // invisible as default, MouseHandler is used instead
	
	m_ratio_cmp = sf::Vector2f(
		getSize().x / (float)sf::VideoMode::getDesktopMode().size.x,
		getSize().y / (float)sf::VideoMode::getDesktopMode().size.y);
}

void Window::Initialize()
{
	std::vector<sf::VideoMode> modes = GetValidModes();

	if (!modes.size())
		throw std::runtime_error("unable to retrieve supported video modes");

	Build(m_border, modes.front(), m_settings);

	if (!setActive(true))
		throw std::runtime_error("window could not be activated");
}

void Window::HandleEvent(const sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		close();
		break;
	case sf::Event::GainedFocus:
		setMouseCursorVisible(false);
		break;
	case sf::Event::LostFocus:
		setMouseCursorVisible(true);
		break;
	case sf::Event::MouseLeft:
		if (hasFocus())
			setMouseCursorVisible(true);
		break;
	case sf::Event::MouseEntered:
		if (hasFocus())
			setMouseCursorVisible(false);
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
	std::vector<sf::VideoMode> modes = GetValidModes();

	if (index >= modes.size())
		throw std::runtime_error("index is out of range");

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
